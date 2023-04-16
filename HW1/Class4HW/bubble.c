#include <stdio.h>
#include <string.h>
#define MAXLENGTH 100       // max length of string input

void getString(char *str);  // helper prototypes
void printResult(char *str);
int greaterThan(char ch1, char ch2);
void swap(char *str, int index1, int index2);

int main(void) {
    int len;                  // length of the entered string
    char str[MAXLENGTH];      // input should be no longer than MAXLENGTH
    // here, any other variables you need

    getString(str);
    len = strlen(str) - 1;        // get length of the string, from string.h
    // put nested loops here to put the string in sorted order
    for (int i = 0; i < len; i++) { //loops through the string
        for (int j = 0; j < len - i; j++) { //loops from the beginning of the string to the point where the last character is sorted
            if (greaterThan(str[j], str[j + 1])) { //checks if the character at the current index is greater than the next character
                swap(str, j, j + 1); //swaps the characters if the first character is greater than the second
            }
        }
    }
    printResult(str);
    return(0);
}

// helper functions go here

// gets the string from the user
void getString(char *str) {
    printf("Enter the string you would like to sort: "); //prints message
    scanf("%s", str); // reads input with scanf and stores it in str which is a pointer to the str defined in main
}

// prints the result
void printResult(char *str) {
    //use printf to print the sorted string from the pointer to the char array
    printf("The sorted string is: %s\n", str);
}

// returns 1 if ch1 is greater than ch2, 0 otherwise
int greaterThan(char ch1, char ch2) {
    if (ch1 > ch2) {
        return 1;
    }
    return 0;
}

//swaps the characters at index1 and index2 in the string
void swap(char *str, int index1, int index2) {
    char temp = str[index1]; //stores the value of the first index in a temp variable
    str[index1] = str[index2];
    str[index2] = temp;
}