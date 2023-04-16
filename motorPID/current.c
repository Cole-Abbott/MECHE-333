#include "nu32dip.h"
#include "current.h"
#include "utils.h"
#include "motor.h"
#include "ina219.h"


void __ISR(_TIMER_3_VECTOR, IPL6SOFT) currentCon(void);

static volatile float dutyCycle;
static volatile float currentRef;
static volatile float Kp = 0.06, Ki = 0.15; //defaut gains
static volatile int testing = 0;
#define EINTMAX 150


//Setup ISR Timer 3, 5kHz
void setupCurrent(void) { //config ISR
	IFS0bits.T3IF = 0;			// Timer3 flag clear
	IEC0bits.T3IE = 1;			// Timer3 enable set
	IPC3bits.T3IP = 6;			// Timer3 priority 6
	IPC3bits.T3IS = 0;			// Timer3 sub-priority 1
	//config Timer3
	T3CONbits.TCKPS = 0;		// Timer3 prescaler = 0 (1:1)
	PR3 = 9600-1;				// period = 48000000/1/5000
	TMR3 = 0; 					// set Timer3 to 0
	T3CONbits.ON = 1;			// turn on Timer3
}


//current control ISR
void currentCon(void) {
    static float e, eint;
    static float newPWM; 
    static int count = 0, REF = 200;
    static float current;

    //choose duty cycle based on mode
    switch(getMode()) {
        case IDLE:
            setPWM(0);
            break;
        case PWM:
            setPWM(dutyCycle);
            break;
        case ITEST: //current PI loop
            testing = 1;
            //get error
            if (count % 25 == 0) { //make REF a square wave by changing every 25 counts == 100Hz
                REF *= -1;
            }
            current = INA219_read_current();
            e = REF - current; // REF???

            //PI calculations
            eint += e;
            if (eint > EINTMAX) { // integrator anti-windup
		        eint = EINTMAX;
	        } else if (eint < -EINTMAX) { 
	        	eint = -EINTMAX;
	        }
            newPWM = Kp * e + Ki * eint;
            setPWM(newPWM);

            //Plot data
            storeCurrent(current, REF);
            count++;
            if (count == IPLTPOINTS - 1) { //stop ITEST once all point done
                setMode(IDLE);
                count = 0;
                testing = 0;
            }
            break;

        case HOLD:
        case TRACK:
        case MANUAL:
            current = INA219_read_current();
            e = currentRef - current; // REF???

            //PI calculations
            eint += e;
            if (eint > EINTMAX) { // integrator anti-windup
		        eint = EINTMAX;
	        } else if (eint < -EINTMAX) { 
	        	eint = -EINTMAX;
	        }
            newPWM = Kp * e + Ki * eint;
            setPWM(newPWM);
            break;
    }
    IFS0bits.T3IF = 0;			// Timer3 flag clear
}


void setDutyCycle(float newDutyCycle) {
    dutyCycle = newDutyCycle;
}

void setCurrentRef(float newRef) {
    currentRef = newRef;
} 

void setCurrentGains(float newKp, float newKi) {
    Kp = newKp;
    Ki = newKi;
}

void getCurrentGains(float *Kpp, float *Kip){
    *Kpp = Kp;
    *Kip = Ki;
}

int isTesting(void){
    return testing;
}
