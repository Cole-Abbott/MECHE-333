#include "utils.h"
#include "nu32dip.h"
#include "position.h"
#define MESSAGESIZE 200

volatile enum mode_t mode; // the declaration


static volatile float IPoints[IPLTPOINTS];
static volatile float IrefPoints[IPLTPOINTS];
static volatile int Icount;

static volatile float PPoints[2000];
static volatile float PrefPoints[2000];
static volatile int Pcount = 0;

char message[200];

enum mode_t getMode(void) {
    return mode;
}

void setMode(enum mode_t newMode) {
    mode = newMode;
}

void storeCurrent(float I, float Iref){
    Icount++;
    Icount = Icount % IPLTPOINTS;
    IPoints[Icount] = I;
    IrefPoints[Icount] = Iref;
}

//send Ipoints, IrefPoints to computer
void plotCurrent(void) {
    for (int i = IPLTPOINTS; i > 0; i--){
        sprintf(message, "%i %f %f\n", i, IPoints[IPLTPOINTS - i], IrefPoints[IPLTPOINTS - i]);
        NU32DIP_WriteUART1(message);
    }
}

void storePosn(float P, float Pref){
    PPoints[Pcount] = P;
    PrefPoints[Pcount] = Pref;
    Pcount++;
}

//send  to computer
void plotPosn(void) {
    int numPoints = getRefLen();
    for(int i = numPoints; i > 0; i--){
        sprintf(message, "%i %f %f\n", i, PPoints[numPoints - i], PrefPoints[numPoints - i]);
        NU32DIP_WriteUART1(message);
        for (int j = 0; j < 10; j++){
            ;
        }
    }
    Pcount = 0;
}