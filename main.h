#ifndef INCLUDE_MAIN_H
#define INCLUDE_MAIN_H

void Write_Command_2_Display(unsigned char command);
void Write_Data_2_Display(unsigned char data);
void Clear_Display(void);
void delay(int delay);

extern int nInterupts;
extern char floatToChar[10];
extern float value;
extern void setupInterupts(int interuptsPerSecond);
extern int loadValue;
extern int tempCount;
extern char buffer[41];

#endif