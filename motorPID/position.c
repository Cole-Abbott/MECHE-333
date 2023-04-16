#include "nu32dip.h"
#include "position.h"
#include "utils.h"
#include "motor.h"
#include "ina219.h"
#include "encoder.h"
#include "current.h"

#define EINTMAX 200
#define SAMPLE_TIME 10

void __ISR(_TIMER_4_VECTOR, IPL5SOFT) posnCon(void);
unsigned int adcRead();					// read ADC

static volatile float current;
static volatile float angleRef;
static volatile float Kp = 400, Ki = 5, Kd = 7000; //defaut gains
static volatile int testing = 0;
static volatile float refArray[2000];
static volatile int refLen;
static volatile int tracking = 0;


char message[200];

//Setup ISR Timer 4, 200Hz
void setupPosn(void) { //config ISR
	IFS0bits.T4IF = 0;			// Timer4 flag clear
	IEC0bits.T4IE = 1;			// Timer4 enable set
	IPC4bits.T4IP = 5;			// Timer4 priority 6
	IPC4bits.T4IS = 0;			// Timer4 sub-priority 1
	//config Timer4
	T4CONbits.TCKPS = 0b011;    // Timer4 prescaler = 010 (1:4)
	PR4 = 30000-1;				// period = 48000000/8/200
	TMR4 = 0; 					// set Timer4 to 0
	T4CONbits.ON = 1;			// turn on Timer4

    //config ADC
	ANSELBbits.ANSB12 = 1;      // AN12 is an adc pin
	AD1CON3bits.ADCS = 1;       // ADC clock period is Tad = 2*(ADCS+1)*Tpb =  4*20.8ns = 83.3ns
  	AD1CON1bits.ADON = 1;       // turn on A/D converter
	
}


//current control ISR
void posnCon(void) {
    static float e, eint, ed, ePrev;
    static float newI; 
    static float angle;
    static int count = 0;

    //choose duty cycle based on mode
    switch(getMode()) {
        case IDLE:
            break;
        case PWM:
            break;
        case ITEST: //current PI loop
            break;
        case TRACK:
            tracking = 1;
            //get error
            WriteUART2("a"); // request the encoder count
            while(!get_encoder_flag()){} // wait for the Pico to respond
            set_encoder_flag(0); // clear the flag so you can read again later
            angle =  (float)get_encoder_count() * 0.09502; // get the encoder value
            e = refArray[count] - angle; 

            //PI calculations
            eint += e;
            ed = e - ePrev;
            if (eint > EINTMAX) { // integrator anti-windup
		        eint = EINTMAX;
	        } else if (eint < -EINTMAX) { 
	        	eint = -EINTMAX;
	        }
            newI = Kp * e + Ki * eint + Kd * ed;
            
            setCurrentRef(newI);
            ePrev = e;

            storePosn(angle, refArray[count]);

            
            if (count == refLen - 1) {
                setMode(HOLD);
                setPosnRef(refArray[count]);
                tracking = 0;
                count = 0;
                break;
            }
            count++;
            break;

        case HOLD:
            //get error
            WriteUART2("a"); // request the encoder count
            while(!get_encoder_flag()){} // wait for the Pico to respond
            set_encoder_flag(0); // clear the flag so you can read again later
            angle =  (float)get_encoder_count() * 0.09502; // get the encoder value
            e = angleRef - angle; 

            //PI calculations
            eint += e;
            ed = e - ePrev;
            if (eint > EINTMAX) { // integrator anti-windup
		        eint = EINTMAX;
	        } else if (eint < -EINTMAX) { 
	        	eint = -EINTMAX;
	        }
            newI = Kp * e + Ki * eint + Kd * ed;
            
            setCurrentRef(newI);
            ePrev = e;
            break;
        case MANUAL:
            //get error
            WriteUART2("a"); // request the encoder count
            while(!get_encoder_flag()){} // wait for the Pico to respond
            set_encoder_flag(0); // clear the flag so you can read again later
            angle =  (float)get_encoder_count() * 0.09502; // get the encoder value
            e = adcRead() - angle; 

            //PI calculations
            eint += e;
            ed = e - ePrev;
            if (eint > EINTMAX) { // integrator anti-windup
		        eint = EINTMAX;
	        } else if (eint < -EINTMAX) { 
	        	eint = -EINTMAX;
	        }
            newI = Kp * e + Ki * eint + Kd * ed;
            
            setCurrentRef(newI);
            ePrev = e;
            break;

    }
    IFS0bits.T4IF = 0;			// Timer4 flag clear
}


void setPosnRef(float newRef) {
    angleRef = newRef;
} 

void setPosnGains(float newKp, float newKi, float newKd) {
    Kp = newKp;
    Ki = newKi;
    Kd = newKd;
}

void getPosnGains(float *Kpp, float *Kip, float *Kid){
    *Kpp = Kp;
    *Kip = Ki;
    *Kid = Kd;
}

void setPosnRefArray(float newRef[], int newRefLen) {
    for (int i = 0; i < newRefLen; i++) {
        refArray[i] = newRef[i];
    }
    refLen = newRefLen;
}


int isTracking(void){
    return tracking;
}

int getRefLen(void) {
    return refLen;
}




// sample & convert the value on pin AN12
unsigned int adcRead() { 
       // adc pin the pin should be configured as an 
       // analog input in ANSELB
    unsigned int elapsed = 0, finish_time = 0;
    AD1CHSbits.CH0SA = 12;                // connect chosen pin to MUXA for sampling
    AD1CON1bits.SAMP = 1;                  // start sampling
    elapsed = _CP0_GET_COUNT();
    finish_time = elapsed + SAMPLE_TIME;
    while (_CP0_GET_COUNT() < finish_time) { 
      ;                                   // sample for more than 250 ns
    }
    AD1CON1bits.SAMP = 0;                 // stop sampling and start converting
    while (!AD1CON1bits.DONE) {
      ;                                   // wait for the conversion process to finish
    }
    return ADC1BUF0;                      // read the buffer with the result
}