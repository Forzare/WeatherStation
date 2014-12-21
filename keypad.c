#include "main.h"
#include "at91sam3x8.h"
#include <stdio.h>
#include <stdlib.h>
#include "servo.h"
#include "printer.h"

int currentMenu = 0;
int temporary;
int buttonPressed;


int readKeypad(){
  int value = 0;
  int column = 0x380;
  int col;
  int row;
  int theRow = -1;
  int theCol = -1;
  
  *AT91C_PIOC_OER = column;
  *AT91C_PIOD_CODR = 1<<2;

  for(col = 1<<7; col <=512; col = col * 2){
    *AT91C_PIOC_SODR = column;
    *AT91C_PIOC_CODR = col;
    theCol++;
    theRow = -1;
    
    for(row = 1<<2; row <= 32; row = row * 2){
      theRow++;
      if((*AT91C_PIOC_PDSR & row) == 0){
        value = theRow*3 + theCol + 1;
        buttonPressed = 1;
      }
      delay(100);
    }
  }
  *AT91C_PIOD_SODR = 1<<2;
  *AT91C_PIOC_ODR = column;
  return value;
}

void menuCases(int *input){
  
  switch(currentMenu){
    
  case 0:
    
    switch(*input){
    
    case 1:
      currentMenu = 1;
      PrintTempTable();
      while(readKeypad()!=11){};
      PrintMenu();
      break;
      
    case 2:
      temporary = findLight();
      sprintf(buffer,"%d \0",temporary);
      Print("Sun at",33,3);
      Print(buffer,33,4);
      
    case 3:
      clearDisplay();
      Print("Set high value", 1, 2);
      
    }
    
  case 1:
    
    switch(*input){
    case 11:
      currentMenu = 0;
      PrintMenu();
    }
  }
}