#ifndef INCLUDE_TEMPERATURE_H
#define INCLUDE_TEMPERATURE_H

typedef struct {
  float min;
  float max;
  float avg;
} table;
extern table* week[7];

extern float logTemp[60];
extern int logTempCount;
extern float tempMin;
extern float tempMax;
extern float tempAvg;
extern float tempTotal;
extern int tempFlag;

void tempMeasure();
void tempCalc();
void updateDegrees();
table* MinMaxAvg(float* tempVec);

#endif