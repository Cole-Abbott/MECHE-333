#include <stdio.h>

int main(void){
    // 0b1101  0b0011
    unsigned char i, a=0x0D, b=0x03;
    i = a<<3;
    i = 0x01 & 0x03;
    printf("%x\n", i);
}
