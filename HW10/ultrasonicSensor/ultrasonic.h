#ifndef ultrasonic__H__
#define ultrasonic__H__

#define maxMessageLength 200

//initialis IO pins
// TRIG: pin B6, output
#define TRIS_TRIG TRISBbits.TRISB6
#define TRIG LATBbits.LATB6

// ECHO: pin B7, input
#define TRIS_ECHO TRISBbits.TRISB8
#define ECHO PORTBbits.RB8

void initUltrasonic(void);
float readUltrasonic(void);

#endif // NU32DIP__H__