#include "helper.h"
#include "nu32dip.h"

int main(void) {
  NU32DIP_Startup();
  Investment inv;

  while(getUserInput(&inv)) {
    inv.invarray[0] = inv.inv0;
    calculateGrowth(&inv);
    sendOutput(inv.invarray,
               inv.years);
  }
  return 0;
}
