#include "system_sam3x.h"
#include "at91sam3x8.h"
#include <stdio.h>
#include "printer.h"
#include "temperature.h"
#include "servo.h"
#include "keypad.h"

#define TC_IDR ((volatile unsigned int *)0x40080028)
#define TC_SR ((volatile unsigned int*)0x40080020)
#define TC_RA ((volatile unsigned int*)0x40080014)
#define TC_RB ((volatile unsigned int*)0x40080018)
#define TC_SR ((volatile unsigned int*)0x40080020)
#define SYSTICK_CTRL ((volatile unsigned int*)0xE000E010)
#define SYSTICK_LOAD ((volatile unsigned int*)0xE000E014)
#define SYSTICK_VAL ((volatile unsigned int*)0xE000E018)
#define ADC_MR ((volatile unsigned int*)0x400C0004)
#define ADC_CHER ((volatile unsigned int*)0x400C0010)
#define ADC_CR ((volatile unsigned int*)0x400C0000)
#define ADC_IDR ((volatile unsigned int*)0x400C0028)
#define ADC_LCDR ((volatile unsigned int*)0x400C0020)
#define ADC_CDR2 ((volatile unsigned int*)0x400C0058)
#define PWM_ENA ((volatile unsigned int*)0x40094004)
#define PWM_CMR ((volatile unsigned int*)0x400942E0)
#define PWM_CPRD ((volatile unsigned int*)0x400942EC)
#define PWM_CDTY ((volatile unsigned int*)0x400942E4)
#define PWM_CDTYUPD ((volatile unsigned int*)0x400942E8)
#define PWM_CCNT ((volatile unsigned int*)0x400942F4)

float value = 0.0;
char theValue = 0;
char buffer[41];
int RB = 0;
int nInterupts = 0;
int count = 0;
int loadValue;
int speedMode = 2160;
char floatToChar[10];

int dayCount = 0;
int tempCount = 0;
int fastMode = 1;

void setupInterupts(int interuptsPerSecond){
  loadValue= 10500000;
  loadValue = loadValue/interuptsPerSecond;
  loadValue = loadValue - 1;
  *SYSTICK_LOAD = loadValue;
  *SYSTICK_VAL = 0;
  nInterupts = 0;
  *SYSTICK_CTRL = 0x3;
}

void delay(int delay){
  int i;
  for(i = 0; i < delay; i++){
    asm("nop");
  }
}

void timerSetup(){
  *SYSTICK_CTRL = 0x0;
  *SYSTICK_VAL = 0;
  *SYSTICK_LOAD = 10499;
    
  *AT91C_PMC_PCER = 1<<12 | 1<<27 | 1<<30;
  
  *AT91C_TC0_CCR = 0x5;
  *AT91C_TC0_CMR = 0x60000;
  *AT91C_TC0_IER = 1<<6;
  
  *AT91C_TC3_CMR = (1 << 14) | 0; //CPCTRG
  *AT91C_TC3_RC = 42000000;
  *AT91C_TC3_IER = 1 << 4;
  *AT91C_TC3_CCR = 0x5;
  
  NVIC_ClearPendingIRQ(TC0_IRQn);
  NVIC_SetPriority(TC0_IRQn, 1);
  NVIC_EnableIRQ(TC0_IRQn);
  
  NVIC_ClearPendingIRQ(TC3_IRQn);
  NVIC_SetPriority(TC3_IRQn, 1);
  NVIC_EnableIRQ(TC3_IRQn);
  
  *AT91C_PIOB_PER = 1<<25;
  *AT91C_PIOB_OER = 1<<25;
  *AT91C_PIOB_PPUDR = 1<<25;
}

void pwmSetup(){
    *AT91C_PIOC_PDR = 1<<24;
    *AT91C_PIOC_ABMR = 1<<24;
    *AT91C_PMC_PCER1 = 1<<4;
    *PWM_ENA = 1<<7;
    *PWM_CMR = 0x5;
    *PWM_CPRD = 52500;
    *PWM_CDTY = 1400;
    *PWM_CDTYUPD = 1400;
}

void adcSetup(){
  *AT91C_PMC_PCER1 = 1<<5; //Enables the PMC on the ADC.
  *ADC_MR = 1<<9;
}

void setup(){
  *AT91C_PMC_PCER = (3<<13) | 1<<11;
  *AT91C_PIOC_PER =  (0xFF3FC);
  
  *AT91C_PIOD_PER = (1<<2)  | 1;
  *AT91C_PIOD_OER = (1<<2)  | 1;
  *AT91C_PIOD_SODR = (1<<2) | 1;
  
  *AT91C_PIOC_ODR = 0x3C;
  *AT91C_PIOC_OER = 0x3F000;
  *AT91C_PIOC_SODR = (1<<12);
  
  //Setup PIOA for the AD2, analog input.
  *AT91C_PIOA_PER = 1<<4;
  *AT91C_PIOA_ODR = 1<<4;
  *AT91C_PIOA_PPUDR = 1<<4;
}

void SysTick_Handler(void){
  nInterupts++;
}

void TC0_Handler(void){
  int x = *AT91C_TC0_SR;
}

void TC3_Handler(void){
  tempCount++;
  int x = *AT91C_TC3_SR;
}

unsigned char Read_Status_Display(void){
  unsigned char temp;
  unsigned int databus = 0x3FC;
  unsigned int dir = 0x2000;
  unsigned int OE = 0x1000;
  unsigned int CD = 0x20000;
  unsigned int CE = 0x10000;
  //unsigned int WR = 0x4000;
  unsigned int RD = 0x8000;
  
  *AT91C_PIOD_SODR = 1<<2;
  *AT91C_PIOC_ODR = databus;
  *AT91C_PIOC_SODR = OE;
  *AT91C_PIOC_SODR = dir;
  *AT91C_PIOC_SODR = CD;
  *AT91C_PIOC_CODR = CE;
  *AT91C_PIOC_CODR = RD;
  delay(25);
  databus = *AT91C_PIOC_PDSR;
  
  databus = databus >> 2;
  
  temp = databus & 3;
  
  *AT91C_PIOC_SODR = CE;
  *AT91C_PIOC_SODR = RD;
  *AT91C_PIOC_SODR = OE;
  *AT91C_PIOC_CODR = dir;
  
  return temp;
}

void Write_Command_2_Display(unsigned char command){
  unsigned int databus = 0x3FC;
  unsigned int dir = 0x2000;
  unsigned int OE = 0x1000;
  unsigned int CD = 0x20000;
  unsigned int CE = 0x10000;
  unsigned int WR = 0x4000;
  
  *AT91C_PIOD_SODR = 1<<2;
  while(Read_Status_Display() != 0x3){
    delay(25);
  }
  *AT91C_PIOC_CODR = databus;
  *AT91C_PIOC_SODR = command<<2;
  *AT91C_PIOC_CODR = dir;
  *AT91C_PIOC_CODR = OE;
  *AT91C_PIOC_OER = databus;
  *AT91C_PIOC_SODR = CD;
  *AT91C_PIOC_CODR = CE;  
  *AT91C_PIOC_CODR = WR;
  delay(25);
  *AT91C_PIOC_SODR = CE;  
  *AT91C_PIOC_SODR = WR;
  
  *AT91C_PIOC_SODR = OE;
  
  *AT91C_PIOC_ODR = databus;
  
}

void Write_Data_2_Display(unsigned char data){
  unsigned int databus = 0x3FC;
  unsigned int dir = 0x2000;
  unsigned int OE = 0x1000;
  unsigned int CD = 0x20000;
  unsigned int CE = 0x10000;
  unsigned int WR = 0x4000;
  
  while(Read_Status_Display() != 0x3){
    delay(25);
  }
  *AT91C_PIOC_CODR = databus;
  *AT91C_PIOC_SODR = data<<2;
  *AT91C_PIOC_CODR = dir; 
  *AT91C_PIOC_CODR = OE;
  *AT91C_PIOC_OER = databus;
  *AT91C_PIOC_CODR = CD;
  *AT91C_PIOC_CODR = CE;
  *AT91C_PIOC_CODR = WR;
  delay(50);
  *AT91C_PIOC_SODR = CE;
  *AT91C_PIOC_SODR = WR;
  *AT91C_PIOC_SODR = OE;
  *AT91C_PIOC_ODR = databus;
}

void Clear_Display(void){
  Write_Data_2_Display(0x00);
  Write_Command_2_Display(0x24);
  Write_Data_2_Display(0x00);
  Write_Command_2_Display(0x24);
  
  for(int i = 0; i<640;i++){
    Write_Data_2_Display(0x00);
    Write_Command_2_Display(0xC0);
  }
  
  Write_Data_2_Display(41);
  Write_Data_2_Display(0);
  Write_Command_2_Display(0x24);
}
 
void Init_Display(void){
  *AT91C_PIOD_CODR = 1;         
  delay(20000);                   
  *AT91C_PIOD_SODR = 1;
  Write_Data_2_Display(0x00);
  Write_Data_2_Display(0x00);
  Write_Command_2_Display(0x40);//Set text home address
  Write_Data_2_Display(0x00);
  Write_Data_2_Display(0x40);
  Write_Command_2_Display(0x42); //Set graphic home address
  Write_Data_2_Display(40);
  Write_Data_2_Display(00);
  Write_Command_2_Display(0x41);// Set text area
  Write_Data_2_Display(0x00);
  Write_Data_2_Display(0x28);
  Write_Command_2_Display(0x43); // Set graphic area
  Write_Command_2_Display(0x80); // text mode
  Write_Command_2_Display(0x94); // Text on graphic off
  Clear_Display();
}

int main(){
  SystemInit();
  setup();
  adcSetup();
  Init_Display();
  timerSetup();
  tempMeasure();    
  updateDegrees();
  setupInterupts(5);
  lightMeasure();
  pwmSetup();
  setupInterupts(1000);
  PrintMenu();
  *PWM_CDTYUPD = 1800;
  int input;
  while(nInterupts < 500){}
  while(1){
        
/*      
    if (readKeypad()==1){
      PrintTempTable();
      while(1){
        if(readKeypad()==11){
          PrintMenu();
          break;
        }
      }
    }
    else if (readKeypad()==2){
      temporary = findLight();
      sprintf(buffer,"%d \0",temporary);
      Print("Sun at",33,3);
      Print(buffer,33,4);
    }*/
    if(nInterupts >= 500){
      tempMeasure();
      nInterupts = 0;
    }
    updateDegrees();
    Print(floatToChar, 33,1); //Skriv ut temperatur
    if(tempCount >= fastMode){
      updateDegrees();
      delay(60);
      if(tempFlag == 1){
        tempCalc();
        tempCount = 0;
        tempFlag = 0;
      }
    }
    input = readKeypad();
    if(buttonPressed == 1){
      buttonPressed = 0;
      menuCases(&input);
    }
  }
}