#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>


char message[13] = "heLlo WoRlD";


int main(){
    int pipefd1[2];
    int pipefd2[2];

    pid_t pid;


    //check possible errors
    //open file descriptors for communication
    if(pipe(pipefd1) == -1){   
        perror("pipe1 error");
        exit(EXIT_FAILURE);
    }

    if(pipe(pipefd2) == -1){   
        perror("pipe2 error");
        exit(EXIT_FAILURE);
    }

    //call fork
    pid = fork();

    if(pid == -1){   
        perror("fork error");
        exit(EXIT_FAILURE);
    }

    //parent
    if (pid != 0) {  
        //close read end for first pipeline
        close(pipefd1[0]);

        //close write end for second pipeline
        close(pipefd2[1]);

        //print first message
        printf("%s\n", message);

        //write from massage into pipe1
        write(pipefd1[1], message, sizeof(message));   
        //close write end     
        close(pipefd1[1]);   

	wait(NULL);

        //read from pipe2 into message
        read(pipefd2[0], message, sizeof(message));
        //close read end
        close(pipefd2[0]);

        //print result
        printf("changed massage is: %s\n", message);

    }//child
    else {
        //close write end for first pipe
        close(pipefd1[1]);
        //close read end for second pipe
        close(pipefd2[0]);


        char changed[strlen(message)];

        //read from pipe1 into changed
        read(pipefd1[0], changed, sizeof(changed));
        //close read end
        close(pipefd1[0]);

        //change the string 
        for(int i=0; i < strlen(changed) ; i++){
            changed[i] = islower(changed[i]) ? toupper(changed[i]) : tolower(changed[i]);
        }

        //write from changed to pipe2
        write(pipefd2[1], changed, sizeof(changed));
        //close write end
        close(pipefd2[1]);  
        }
    return 0;
}
