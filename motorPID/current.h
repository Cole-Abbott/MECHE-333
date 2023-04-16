#ifndef CURRENT__H__
#define CURRENT__H__

//setup current controll ISR
void setupCurrent(void);

//set new duty cycle
void setDutyCycle (float);

//set new current REF
void setCurrentRef (float);

//set current gains
void setCurrentGains(float, float);

//get current gains
void getCurrentGains(float*, float*);

int isTesting(void);

#endif