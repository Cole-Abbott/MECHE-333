//wait for comands from client and act acordingly
#include "nu32dip.h"
#include "motor.h"

//sets up Connection to H-Bridge
//PWM on OC1 on pin B7 using timer2
void setupMotor(void) {
    //config pwm from OC1 based on timer2
	T2CONbits.TCKPS = 0;      	// Timer2 prescaler N=0 (1:1)
	PR2 = 2400-1;             	// period = 2400 ticks, 20 kHz
	TMR2 = 0;                 	// initial TMR3 count is 0
	OC1CONbits.OCM = 0b110;   	// PWM mode without fault pin
	OC1CONbits.OCTSEL = 0;    	// set OC1 to use Timer2 as the time base
	RPB7Rbits.RPB7R = 0b0101; 	// set pin RB7 to OC1 output
	OC1RS = 0;              	// duty cycle = OC1RS/(PR3+1) = 25%
	OC1R = 0;               	// initialize before turning OC1 on; afterward it is read-only
	T2CONbits.ON = 1;         	// turn on Timer3
	OC1CONbits.ON = 1;        	// turn on OC1

    TRISBbits.TRISB6 = 0;       //set B6 as output for direction
    LATBbits.LATB6 = 1;
}

//sets PWM duty cycle to the motor to the specified percent
//sign indicates direction
void setPWM(float PWM) {
    //scale -100 to 100 onto 0-2399 and direction bit
    if (PWM > 0) {
        LATBbits.LATB6 = 0;
        if (PWM > 100) {PWM = 100;}
        OC1RS = (int)(23.9 * PWM);
    } else if (PWM < 0) {
        LATBbits.LATB6 = 1;
        if (PWM < -100) {PWM = -100;}
        OC1RS = (int)(-23.9 * PWM);
    } else {
        OC1RS = 0;
    }
}
