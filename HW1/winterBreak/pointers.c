#include <stdio.h>


int main(void)
{
    int x, *xp;
    xp = &x;
    //x = 5;
    printf("%i\n", *xp);
    printf("%p\n", xp);
}