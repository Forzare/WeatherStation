#ifndef INCLUDE_KEYPAD_H
#define INCLUDE_KEYPAD_H

int readKeypad();
void menuCases(int *input);

extern int buttonPressed;
extern int maxLimit;
extern int lowLimit;

#endif