#include "nu32dip.h" 
#include "ultrasonic.h"         // constants, funcs for startup and UART

//initialis IO pins
// TRIG: pin B6, output
// ECHO: pin B7, input
void initUltrasonic(void) {
    // b6 output to trig
    // b7 input from echo
    TRIS_TRIG = 0; // trig to output
    TRIS_ECHO = 1; // echo to input
    TRIG = 0; //set trig low
}

//returns distance from sensor in meters
float readUltrasonic(void) {
    //tell sensor to read
    TRIG = 1; //set trig high
    // wait 10us
    _CP0_SET_COUNT(0);
    while(_CP0_GET_COUNT() < 24000000*0.00001) {}
    TRIG = 0; //set trig low

    //wait for echo to go high
    while(ECHO == 0) {}

    //time how long echo is high
    _CP0_SET_COUNT(0);
    while(ECHO == 1 && _CP0_GET_COUNT() < 24000000 * 1) {}
    unsigned int t = _CP0_GET_COUNT();

    //convert to distance and return
    float d = (t/2.0) / 24000000 * 340;
    return d;
}