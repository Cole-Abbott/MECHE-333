#include "nu32dip.h" 
#include "ultrasonic.h" 
int main(void) {  
    char message[maxMessageLength];
    NU32DIP_Startup(); // cache on, interrupts on, LED/button init, UART init
    NU32DIP_WriteUART1("ultrasonic.c\r\n");
    
    //init the sensor
    initUltrasonic();
    
    float d;

    while (1) {        
        d = readUltrasonic();
        // print the value of t
        if (d < 4){
            sprintf(message, "d = %f\r\n", d);
            NU32DIP_WriteUART1(message);
        }

    }
    return 0;
}