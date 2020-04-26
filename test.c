#include <stdio.h>
#include <string.h>


int main() {

    char buff[] = "HOLAAA";

    memcpy(0, buff, sizeof(buff));


    return 0;
}