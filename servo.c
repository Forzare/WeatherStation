#include "servo.h"
#include "main.h"
#include "at91sam3x8.h"

#define PWM_ENA ((volatile unsigned int*)0x40094004)
#define PWM_CMR ((volatile unsigned int*)0x400942E0)
#define PWM_CPRD ((volatile unsigned int*)0x400942EC)
#define PWM_CDTY ((volatile unsigned int*)0x400942E4)
#define PWM_CDTYUPD ((volatile unsigned int*)0x400942E8)

#define ADC_CHER ((volatile unsigned int*)0x400C0010)
#define ADC_CR ((volatile unsigned int*)0x400C0000)
#define ADC_LCDR ((volatile unsigned int*)0x400C0020)
#define ADC_CDR2 ((volatile unsigned int*)0x400C0058)

int lightValueLeft = 0;
int lightValueRight = 0;

int findLight(){
    while(nInterupts <2){};
    int minimumLight = 4095;
    int currentLight;
    unsigned int thePosition;
    unsigned int servo;
    for(servo = 1500; servo < 5801; servo = servo + 100){
      currentLight = lightMeasure();
      if(currentLight < minimumLight){
        minimumLight = currentLight;
        thePosition = servo;
      }
      setupInterupts(1000);
      while(nInterupts < 40){} 
        *PWM_CDTYUPD = servo;   
      }
    setupInterupts(1000);
    while(nInterupts < 100){} 
    *PWM_CDTYUPD = thePosition;
    return thePosition;
}

void lightMeasureLeft(){
  *ADC_CHER  = 1<<2;
  *ADC_CR = 1<<1;
  //int lightValue = *ADC_CDR2;
  lightValueLeft = *ADC_CDR2;
  *ADC_CR = 1<<1;
  lightValueLeft = *ADC_LCDR & 0xFFF;
  //return lightValue; 
}

int lightMeasure(){
  *ADC_CHER  = 1<<2;
  *ADC_CR = 1<<1;
  int lightValue = *ADC_CDR2;
  *ADC_CR = 1<<1;
  lightValue = *ADC_LCDR & 0xFFF;
  return lightValue; 
}


