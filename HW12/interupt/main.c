#include "nu32dip.h"
#define MAXMESSAGE 200
volatile int state; // 0: timer stopped, 1: timer running


void __ISR(_EXTERNAL_0_VECTOR, IPL6SOFT) Ext0ISR(void) {
    int time = _CP0_GET_COUNT();
    char message[MAXMESSAGE];

    //debounce button
    while(_CP0_GET_COUNT() < time + 240000) { ; } //delay 10ms
    //read pin, if pressed, keep going, else break
    if (PORTBbits.RB7 == 1) {return;}

    if (state == 0){ //start timer
        _CP0_SET_COUNT(0);
        NU32DIP_WriteUART1("Press button to stop timer \n\r");
    } else if (state == 1) { //stop timer
        float s = (float)time / 24000000.0;
        sprintf(message, "time: %4.2f\r\n", s);
        NU32DIP_WriteUART1(message);
        NU32DIP_WriteUART1("Press the button to start the timer\r\n");
    }

    state = !state;

    IFS0bits.INT0IF = 0; //clear interrupt flag 
    NU32DIP_GREEN = !NU32DIP_GREEN;  //invert NU32_DIP_GREEN
}

//LAP function
void __ISR(_EXTERNAL_1_VECTOR, IPL6SOFT) Ext1ISR(void) {
    int time = _CP0_GET_COUNT();
    //debounce button
    while(_CP0_GET_COUNT() < time + 240000) { ; } //delay 10ms
    //read pin, if pressed, keep going, else break
    if (PORTBbits.RB10 == 1) {return;}

    if (state == 1) { //if timer is running, print out current time
        char message[MAXMESSAGE];
        float s = (float)time / 24000000.0;
        sprintf(message, "Lap: %4.2f\r\n", s);
        NU32DIP_WriteUART1(message);
   }
    IFS0bits.INT1IF = 0; //clear interrupt flag 
}

int main(void) {

    NU32DIP_Startup();
    __builtin_disable_interrupts(); //disable interrupts

    //config INT0
    INTCONbits.INT0EP = 0; //trigger on falling edge
    IPC0bits.INT0IP = 6; //interrupt priority 6
    IPC0bits.INT0IS = 1; //interrupt sub priority 1
    IFS0bits.INT0IF = 0; //clear interrupt flag 
    IEC0bits.INT0IE = 1; //enable interrupt

    //config INT1
    INTCONbits.INT1EP = 0; //trigger on falling edge
    IPC1bits.INT1IP = 6; //interrupt priority 6
    IPC1bits.INT1IS = 1; //interrupt sub priority 1
    IFS0bits.INT1IF = 0; //clear interrupt flag 
    IEC0bits.INT1IE = 1; //enable interrupt
    //INT1 on RPB10
    INT1Rbits.INT1R = 3;

    __builtin_enable_interrupts();

    NU32DIP_WriteUART1("Press the button to start the timer\r\n");
    state = 0;
    
}