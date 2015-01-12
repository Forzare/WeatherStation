#ifndef INCLUDE_DISPLAY_H
#define INCLUDE_DISPLAY_H

unsigned char Read_Status_Display(void);
void Clear_Display(void);
void Write_Command_2_Display(unsigned char command);
void Write_Data_2_Display(unsigned char data);

#endif