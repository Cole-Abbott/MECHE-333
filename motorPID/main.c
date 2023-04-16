//wait for comands from client and act acordingly
#include "nu32dip.h"
#include "motor.h"
#include "encoder.h"
#include "utils.h"
#include "ina219.h"
#include "current.h"
#include "position.h"


#define MESSAGESIZE 200

void main(void) {
    //setup
    NU32DIP_Startup();           // cache on, interrupts on, LED/button init, UART init
	NU32DIP_WriteUART1("main.c\n\r");

    //setup connections to peripherals
    __builtin_disable_interrupts();
        setupMotor();
        UART2_Startup(); 
        setMode(IDLE);
        INA219_Startup();
        setupCurrent();
        setupPosn();
	__builtin_enable_interrupts();


    NU32DIP_GREEN = 1;
    NU32DIP_YELLOW = 1;
    char message[MESSAGESIZE];
    
    
    while (1){ //infinite loop
        //read UART
        NU32DIP_ReadUART1(message, MESSAGESIZE);
        NU32DIP_YELLOW = 1; //clear led
        //switch statement
        switch (message[0])
        {

        int p;
        case 'c': //read encoder (counts)
            WriteUART2("a"); // request the encoder count
            while(!get_encoder_flag()){} // wait for the Pico to respond
            set_encoder_flag(0); // clear the flag so you can read again later
            p = get_encoder_count(); // get the encoder value
            sprintf(message, "The encoder count is: %d counts\n", p);
            NU32DIP_WriteUART1(message);
            break;

        case 'd': //read encoder (deg)
            WriteUART2("a"); // request the encoder count
            while(!get_encoder_flag()){} // wait for the Pico to respond
            set_encoder_flag(0); // clear the flag so you can read again later
            p = get_encoder_count(); // get the encoder value
            sprintf(message, "The encoder count is: %f deg\n", ((float)p/3360.0) * 360.0);
            NU32DIP_WriteUART1(message);
            break;

        case 'e': //reset encoder
            WriteUART2("b");
            break;

        case 'r': //send mode
            switch (getMode()){
                case IDLE:
                    NU32DIP_WriteUART1("IDLE\n");
                    break;
                case PWM:
                    NU32DIP_WriteUART1("PWM\n");
                    break;
                case ITEST:
                    NU32DIP_WriteUART1("ITEST\n");
                    break;
                case HOLD:
                    NU32DIP_WriteUART1("HOLD\n");
                    break;
                case TRACK:
                    NU32DIP_WriteUART1("TRACK\n");
                    break;
                case MANUAL:
                    NU32DIP_WriteUART1("MANUAL\n");
            }
            break;
        
        float current;
        case 'a': //read current sensor (mA)
            current = INA219_read_current();
            sprintf(message, "Current: %4.2fmA\n", current);
            NU32DIP_WriteUART1(message);
            break;

        float PWMduty;
        case 'f': //set PWM
            NU32DIP_ReadUART1(message, MESSAGESIZE);
            sscanf(message, "%f", &PWMduty);
            setMode(PWM);
            setDutyCycle(PWMduty);
            break;
        
        case 'p': //turn of motor
            setMode(IDLE);
            break;

        float Kp, Ki;
        case 'h': //get current gains
            getCurrentGains(&Kp, &Ki);
            sprintf(message, "Kp: %4.2f  Ki: %4.2f\n", Kp, Ki);
            NU32DIP_WriteUART1(message);
            break;

        float newKp, newKi;
        case 'g': //set current gains
            NU32DIP_ReadUART1(message, MESSAGESIZE);
            sscanf(message, "%f %f", &newKp, &newKi);
            setCurrentGains(newKp, newKi);
            break;


        case 'k': //test current gains
            setMode(ITEST);
            //wait for Itest to finish
            while(isTesting()){
                ;
            }
            //send data to client
            plotCurrent();
            break;


        float posnKp, posnKi, posnKd;
        case 'j': //get posn gains
            getPosnGains(&posnKp, &posnKi, &posnKd);
            sprintf(message, "Kp: %4.2f  Ki: %4.2f  Kd: %4.2f\n", posnKp, posnKi, posnKd);
            NU32DIP_WriteUART1(message);
            break;

        float newPosnKp, newPosnKi, newPosnKd;
        case 'i': //set posn gains
            NU32DIP_ReadUART1(message, MESSAGESIZE);
            sscanf(message, "%f %f %f", &newPosnKp, &newPosnKi, &newPosnKd);
            setPosnGains(newPosnKp, newPosnKi, newPosnKd);
            break;

        float angle;
        case 'l': //go to angle
            NU32DIP_ReadUART1(message, MESSAGESIZE);
            sscanf(message, "%f", &angle);
            setPosnRef(angle);
            setMode(HOLD);
            break;

        case 'q': //quit, return to idle state
            setMode(IDLE);
            break;

        static float refArray[2000];
        static int refLen = 0;
        case 'm': //load step ref
        case 'n': //load cubic ref
            NU32DIP_ReadUART1(message, MESSAGESIZE); //read array len
            sscanf(message, "%d", &refLen);
            for (int i = 0; i < refLen; i++){  
                NU32DIP_ReadUART1(message, MESSAGESIZE);
                sscanf(message, "%f", &refArray[i]);
            }
            setPosnRefArray(refArray, refLen);
            break;

        case 'o': //execute trajecotry
            setMode(TRACK);
            while(getMode() == TRACK) {
                ;
            }
            plotPosn();

            break;
        case 'b': 
            setMode(MANUAL);
            break;


        default: //turn on led to indicate incorect command
            NU32DIP_YELLOW = 0;
            break;
        }
    }
}

