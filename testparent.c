#include <stdio.h> 
#include <string.h> 
#include <fcntl.h> 
#include <sys/stat.h> 
#include <sys/types.h> 
#include <unistd.h> 
#include <stdlib.h>
  
int main() {
    printf("Parent started\n");

    int fd[2];
    pipe(fd);

    int stdoutValue = dup(1);

    if (fork()) {
    	printf("Child started\n");
    	dup2(fd[0], 0);
        dup2(fd[1], 1);

        static char *argv[]={"arg1","arg2",NULL};
        execv("testchild",argv);

        //Child finished, kill this fork
		exit(0);
	}

    printf("Sending message to child\n");
    dup2(fd[1], 1);
    printf("sss");
    printf("kok\n");
    dup2(stdoutValue, 1);
	printf("Reading message from child\n");
	dup2(fd[0], 0);

	char inbuf[32]; 

	read(fd[0], inbuf, 32); 
	printf("%s\n", inbuf);

    return 0; 
} 
