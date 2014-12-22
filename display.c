#include "main.h"
#include "at91sam3x8.h"

unsigned char Read_Status_Display(void){
  unsigned char temp;
  unsigned int databus = 0x3FC;
  unsigned int dir = 0x2000;
  unsigned int OE = 0x1000;
  unsigned int CD = 0x20000;
  unsigned int CE = 0x10000;
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

