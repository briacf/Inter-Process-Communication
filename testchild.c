
#include <stdio.h> 
#include <string.h> 
#include <fcntl.h> 
#include <sys/stat.h> 
#include <sys/types.h> 
#include <unistd.h> 
  
int main() {
    char inbuf[32]; 

    read(0, inbuf, 32); 
    printf("yousent%s", inbuf);

    printf("lmao\n");

    return 0; 
} 
