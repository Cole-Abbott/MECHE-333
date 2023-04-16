#ifndef POSITION__H__
#define POSITION__H__

//setup current controll ISR
void setupPosn(void);

//set new duty cycle
//void setDutyCycle (float);

//set new current REF
void setPosnRef (float);

//set current gains
void setPosnGains(float, float, float);

//get current gains
void getPosnGains(float*, float*, float*);

//set Posn Ref Array
void setPosnRefArray(float[], int);

int isTracking(void);
int getRefLen(void);

#endif