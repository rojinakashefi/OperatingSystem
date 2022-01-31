#include <stdio.h>
#include <stdio_ext.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <signal.h>

#define PORT 3232// default port number
#define MAXDATALEN 256

int clientFD;
int n;
struct sockaddr_in serv_addr; //structure to hold server's address
char buffer[MAXDATALEN];
char clientName[10];

void *writeToChat(int);
void *readFromChat(int);
void *quit();

int main(int argc, char *argv[])
{

    pthread_t readThread, writeThead; /* variable to hold thread ID */
    //number of argumnets needed
    if (argc != 4)
    {
        printf("you should enter server ip, port and username to connect.\n");
        exit(1);
    }

    // socket creating
    clientFD = socket(AF_INET, SOCK_STREAM, 0);
    if (clientFD == -1)
        printf("client socket error\n");
    else
        printf("socket created\n");

    // set info
    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);

    // username
    bzero(clientName, 10);
    strcpy(clientName, argv[3]);
    clientName[strlen(clientName)] = ' ';
    // client connect to server
    if (connect(clientFD, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
    {
        printf("connection failed\n");
        exit(0);
    }
    else
        printf(" %s! you joind chat\n", clientName);

    send(clientFD, clientName, strlen(clientName), 0);

    //creating threads
    pthread_create(&writeThead, NULL, (void *)writeToChat, (void *) (intptr_t) clientFD); //thread for writing
    pthread_create(&readThread, NULL, (void *)readFromChat, (void *) (intptr_t) clientFD);  //thread for reading
    pthread_join(writeThead, NULL);
    pthread_join(readThread, NULL);

    return 0;

}

void *readFromChat(int clientFD)
{
    signal(SIGINT,(void *)quit);
    while (1)
    {
        n = recv(clientFD, buffer, MAXDATALEN - 1, 0);
        if (n == 0)
        {
            printf("server shut down!\n");
            exit(0);
        }

        if (n > 0)
        {
            printf("new massage from %s :", buffer);
            bzero(buffer, MAXDATALEN);
        }
    }
}

void *writeToChat(int clientFD)
{
    while (1)
    {
        fgets(buffer, MAXDATALEN - 1, stdin);

        if (strlen(buffer) - 1 > sizeof(buffer))
        {
            printf("buffer is full. max size of bffer is %ld bytes\n", sizeof(buffer));
            bzero(buffer, MAXDATALEN);
            __fpurge(stdin);
        }

        n = send(clientFD, buffer, strlen(buffer), 0);

        if (strncmp(buffer, "/quit", 5) == 0)
            exit(0);

        bzero(buffer, MAXDATALEN);
    }
}

// handling SIGINT
void *quit()
{
    printf("Type '/quit' to exit \n");
}
