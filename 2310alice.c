#include <stdio.h> 
#include <string.h> 
#include <fcntl.h> 
#include <sys/stat.h> 
#include <sys/types.h> 
#include <unistd.h> 
  
int main(int argc, char *argv[]) {
	printf("Player here\n");
	char inbuf[32]; 
	read(0, inbuf, 32);

	printf("Message: %s\n", inbuf);

    return 0; 
} 
