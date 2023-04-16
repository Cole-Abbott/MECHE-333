#ifndef UTILS__H__
#define UTILS__H__

enum mode_t {IDLE, PWM, ITEST, HOLD, TRACK, MANUAL}; // the definition

enum mode_t getMode(void);
void setMode(enum mode_t newMode);
enum mode_t getMode(void);

#define IPLTPOINTS 200 //current points to plot
//store Iref, I
void storeCurrent(float, float);


void plotCurrent(void);

void storePosn(float, float);
void plotPosn(void);

#endif // UTILS__H__
