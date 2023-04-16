#ifndef helper__H__
#define helper__H__

#define MAX_YEARS 100

typedef struct {
  double inv0;
  double growth;
  int years;
  double invarray[MAX_YEARS+1];   
} Investment;

int getUserInput(Investment *invp); 
void calculateGrowth(Investment *invp);
void sendOutput(double *arr, int years);

#endif //helper__H__