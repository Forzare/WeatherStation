#include "printer.h"
#include "main.h"
#include "temperature.h"
#include <stdio.h>

char tableValues[40];

void Print(char* text, int x, int y){
  unsigned short coord = (y*40 + x);
  Write_Data_2_Display(coord);
  Write_Data_2_Display(coord >> 8);
  Write_Command_2_Display(0x24);
  while(*text != 0) {
    Write_Data_2_Display(*text-0x20);
    Write_Command_2_Display(0xC0);
    text++;
  }
}

void PrintMenu(void){
  Clear_Display();
  Print("1. Temperature table", 0,1);
  Print("2. Find position of the sun",0,3);
  Print("3. Set temperature alarm",0,5);
  Print("4. ",0,7);
  Print("5. ",0,9);
  Print("6. Toggle fast mode",0,11);
}

void PrintTempTable(void){
  Clear_Display();
  Print("----------------------------------------",0,0);
  Print("     |Mon |Tue |Wed |Thu |Fri |Sat |Sun ",0,1);
  Print("----------------------------------------",0,2);
  Print(" Max |    |    |    |    |    |    |    ",0,3);
  Print(" Avg |    |    |    |    |    |    |    ",0,4);
  Print(" Min |    |    |    |    |    |    |    ",0,5);
  Print("----------------------------------------",0,6);
  Print("Go back to main menu by pressing 0.",0,14);
  int i=0;
  for(i=0; i<7; i++){
    sprintf(tableValues,"%.1f",week[i]->max);
    tableValues[4] = 0;
    Print(tableValues,(6+i*5),3);
    sprintf(tableValues,"%.1f",week[i]->avg);
    tableValues[4] = 0;
    Print(tableValues,(6+i*5),4);
    sprintf(tableValues,"%.1f",week[i]->min);
    tableValues[4] = 0;
    Print(tableValues,(6+i*5),5);
  }
}