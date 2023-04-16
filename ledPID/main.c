#include "NU32dip.h"          // constants, functions for startup and UART

#define NUMSAMPS 1000 					// number of points in waveform
#define SAMPLE_TIME 10 					// time to sample ADC
#define PLOTPTS 200 					// number of data points to plot
#define DECIMATION 10					// plot every 10th point
#define MESSAGESIZE 100 
#define EINTMAX 400

//global Vars
static volatile int Waveform[NUMSAMPS]; // waveform
static volatile int ADCarray[PLOTPTS];  // measured values to plot
static volatile int REFarray[PLOTPTS];  // reference values to plot
static volatile int ERRarray[PLOTPTS];  // error values to plot
static volatile int DUTYarray[PLOTPTS];  // duty values to plot


static volatile int StoringData = 0; 	// if this flag = 1, currently storing
static volatile int counter = 0;		// waveform counter
static volatile float Kp = 0.125, Ki = 0.018; // PI values
static volatile int eint;				// eint global so it can be reset in main

//functions
void makeWaveform(void);				// init waveform
unsigned int adcRead();					// read ADC

//set Controller to Timer2 ISR
void __ISR(_TIMER_2_VECTOR, IPL6SOFT) controller(void);

//config Timer3,OC1 and Timer2,ISR
int main(void) {
	NU32DIP_Startup();           // cache on, interrupts on, LED/button init, UART init
	NU32DIP_WriteUART1("main.c\n\r");

	//config pwm from OC1 based on timer3 
	{
	T3CONbits.TCKPS = 0;      	// Timer3 prescaler N=0 (1:1)
	PR3 = 2400-1;             	// period = 2400 ticks, 20 kHz
	TMR3 = 0;                 	// initial TMR3 count is 0
	OC1CONbits.OCM = 0b110;   	// PWM mode without fault pin
	OC1CONbits.OCTSEL = 1;    	// set OC1 to use Timer3 as the time base
	RPB7Rbits.RPB7R = 0b0101; 	// set pin RB7 to OC1 output
	OC1RS = 1800;              	// duty cycle = OC1RS/(PR3+1) = 25%
	OC1R = 1800;               	// initialize before turning OC1 on; afterward it is read-only
	T3CONbits.ON = 1;         	// turn on Timer3
	OC1CONbits.ON = 1;        	// turn on OC1
	}

	//config ISR on Timer2 @ 1kHz
	{
	__builtin_disable_interrupts();
	//config ISR
	IFS0bits.T2IF = 0;			// Timer2 flag clear
	IEC0bits.T2IE = 1;			// Timer2 enable set
	IPC2bits.T2IP = 6;			// Timer2 priority 6
	IPC2bits.T2IS = 0;			// Timer2 sub-priority 1
	//config Timer2
	T2CONbits.TCKPS = 0;		// Timer2 prescaler = 0 (1:1)
	PR2 = 48000-1;				// period = 48000, 1kHz
	TMR2 = 0; 					// set Timer2 to 0
	T2CONbits.ON = 1;			// turn on Timer2
	__builtin_enable_interrupts();
	}

	//config ADC
	{
	ANSELBbits.ANSB12 = 1;      // AN12 is an adc pin
	AD1CON3bits.ADCS = 1;       // ADC clock period is Tad = 2*(ADCS+1)*Tpb =  4*20.8ns = 83.3ns
  	AD1CON1bits.ADON = 1;       // turn on A/D converter
	}
	
	//variables for main loop
	makeWaveform();
	char message[MESSAGESIZE];
	float kptemp, kitemp;
	int i = 0;
	while(1){ //infinite loop
		//wait to recive new Kp, Ki values
		NU32DIP_ReadUART1(message, MESSAGESIZE);
		sscanf(message, "%f %f", &kptemp, &kitemp);
		//set new values
		__builtin_disable_interrupts();
		Kp = kptemp;
		Ki = kitemp;
		__builtin_enable_interrupts();
		StoringData = 1;
		eint = 0;
		
		while(StoringData) { //do nothing while storing data
			;
		}
		//send data back to computer
		for (i = 0; i < PLOTPTS; i++) {
			//when first number is 1, client will know we are done
			sprintf(message, "%d %d %d %d %d\r\n", PLOTPTS - i - 1, ADCarray[i], REFarray[i], ERRarray[i], DUTYarray[i]);
			NU32DIP_WriteUART1(message);
		}
		
	}
	return 0;
}

//inits a square waveform
void makeWaveform(void) {
	int i, center = 512, A = 200; //set center, amplitude of waveform
	for(i = 0; i < NUMSAMPS; i++) {
		if(i < NUMSAMPS / 2) { Waveform[i] = center + A; }
		else { Waveform[i] = center - A; }
	}
}

//sets OC1 duty cycle to next val in Waveform
void controller(void) {
	static int Vout = 0;
	static int dataCounter = 0;
	static float e;
	static float u, unew;
	counter = (counter + 1) % NUMSAMPS; //inc counter

	//read Vout
	Vout = adcRead();

	//calc error
	e = Waveform[counter] - Vout;
	//calc PI
	eint = eint + e;

	if (eint > EINTMAX) { // integrator anti-windup
		eint = EINTMAX;
	} else if (eint < -EINTMAX) { 
		eint = -EINTMAX;
	}

	u = Kp*e + Ki*eint;

	//set PWM duty cycle
	unew = u + 50.0;
	if (unew > 100.0) {
	unew = 100.0;
	NU32DIP_GREEN = !NU32DIP_GREEN;
	} else if (unew < 0.0) {
	NU32DIP_YELLOW = !NU32DIP_YELLOW;
	unew = 0.0;
	}
	OC1RS = (unsigned int) ((unew/100.0) * PR3);

	//store data every DECIMATION points
	if (StoringData && ((counter % DECIMATION) == 0)) {
		//store values to plot
		ADCarray[dataCounter] = Vout;
		REFarray[dataCounter] = Waveform[counter];
		ERRarray[dataCounter] = e;
		DUTYarray[dataCounter] = unew;
		dataCounter++;
		if (dataCounter == PLOTPTS) {
			StoringData = 0;
			dataCounter = 0;
		}
	}
	
	IFS0bits.T2IF = 0;			//Tieintmer2 flag clear
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