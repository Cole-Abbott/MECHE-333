#include <stdio.h>  
#include "nu32dip.h"      
#define MAX_YEARS 100
#define maxMessageLength 200

typedef struct {
  double inv0;
  double growth;
  int years;
  double invarray[MAX_YEARS+1];   
} Investment;

int getUserInput(Investment *invp); 
void calculateGrowth(Investment *invp);
void sendOutput(double *arr, int years);

int main(void) {

  Investment inv;

  while(getUserInput(&inv)) {
    inv.invarray[0] = inv.inv0;
    calculateGrowth(&inv);
    sendOutput(inv.invarray,
               inv.years);
  }
  return 0;
}

void calculateGrowth(Investment *invp) {

  int i;

  for (i = 1; i <= invp->years; i= i + 1) {
    invp->invarray[i] = invp->growth * invp->invarray[i-1]; 
  }
} 

int getUserInput(Investment *invp) {
  //message array for input and output
  char message[maxMessageLength];
  int valid;

  //write message into char array and send to computer
  sprintf(message, "Enter investment, growth rate, number of yrs (up to %d): ",MAX_YEARS);
  NU32DIP_WriteUART1(message);

  //read from computer and  write into message, then use sscanf to parse into variables
  NU32DIP_ReadUART1(message, maxMessageLength);
  sscanf(message, "%lf %lf %d", &(invp->inv0), &(invp->growth), &(invp->years));

  valid = (invp->inv0 > 0) && (invp->growth > 0) &&
    (invp->years > 0) && (invp->years <= MAX_YEARS);

  //write message into char array and send to computer
  sprintf(message, "Valid input?  %d\r\n",valid);
  NU32DIP_WriteUART1(message);

  if (!valid) { 
    //send string to computer
    NU32DIP_WriteUART1("Invalid input; exiting.\r\n");
  }
  return(valid);
}


void sendOutput(double *arr, int yrs) {

  int i;
  char outstring[100];  
  //send string to computer
  NU32DIP_WriteUART1("\nRESULTS:\r\n\n");
  for (i=0; i<=yrs; i++) {  
    //write message into char array and send to computer
    sprintf(outstring,"Year %3d:  %10.2f\r\n",i,arr[i]); 
    NU32DIP_WriteUART1(outstring);
  }
  //send string to computer
  NU32DIP_WriteUART1("\r\n");
} 
