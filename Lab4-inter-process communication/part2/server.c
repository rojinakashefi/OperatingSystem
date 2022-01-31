#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <netinet/in.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>

typedef struct
{
    int port;
    char username[10];
} User;

#define PORT 3232// default port number
#define MAXDATALEN 256 // max size of messages to be sent
#define MAXUSERS 10 // max number of users
#define MAXGROUPS 5// max number of groups

void *clientHandler(void *arg);//client handler for every connected client
void insertMember(int port, char *username, User *list, int *tail); //adds new clients to a group
int searchMember(int port, User *list, int tail); //seraches to see if a client is a member of a group or not 
void leftGroup(int port, User *list, int *tail); // delets a user from groups when he left
void showOnlines(const User *list, int tail); //shows all clients in the chat
void notify_shutdown();//notify all clients if server shuts down
void *quit_signal();//signal handler
int next_space(char *str);

char username[10];
User users[MAXUSERS] = {0}; //list of all users
int userIndex = 0; //holds last index in user list(which is not 0), so a new member could be directley added to list 
User groups[MAXGROUPS][MAXUSERS] = {0}; //list of all groups in chat(each group can have maximum size of 10) and there are at most 5 gps
int group_tail[MAXUSERS] = {0};
char buffer[MAXDATALEN];

int main(int argc, char *argv[])
{
    struct sockaddr_in serverAddress; //sockaddr-in is basic structure for all syscalls and and functions deal with internet addresses
    struct sockaddr_in client_addr;
    int ServerFD, ClientFD; 
    int portNumber; 
    pthread_t thr;
    int clientSize;
    int yes = 1;

    printf("server starting: \n");
    if (argc == 2) //if number of entered arguments was 2 then second argument should be portnumber (we cast it to int)
        portNumber = atoi(argv[1]);
    else
        portNumber = PORT; //if port number not given as argument then using default port is 3232
    printf("port number: %d\n", portNumber);

    //server info
    serverAddress.sin_family = AF_INET;// set family to Internet 
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY); // set IP address
    serverAddress.sin_port = htons(portNumber);
    printf("ip: %s\n", inet_ntoa(serverAddress.sin_addr));

    //creating socket
    ServerFD = socket(AF_INET, SOCK_STREAM, 0);

    //binding socket
    if (bind(ServerFD, (struct sockaddr *)&serverAddress, sizeof(struct sockaddr)) < 0)
    {
        printf("binding failed!\n");
        exit(1);
    }

    //listening on server socket with backlog sie 100
    if(listen(ServerFD, 100) < 0){
        printf("listening failed!\n");
        exit(1);
    }
    printf("listening for clients...\n");


    //signals are interrupts that force os to stop its ongoing task
    //first param:an int(signal number),produces receceipt for an active signal , second : signal handler
    signal(SIGINT, (void *)quit_signal);


    // in this while we can accept new clients till we reach max number and create thread for each
    while (1)
    {
        clientSize = sizeof(struct sockaddr_in); 
        ClientFD = accept(ServerFD, (struct sockaddr *)&client_addr, &clientSize); //accepting connection from client

        bzero(username, 10);
        int t = recv(ClientFD, username, sizeof(username), 0);
	username[strlen(username)-1] = ' ';
        printf(" %s(port %d) joined\n", username, ClientFD);
        insertMember(ClientFD, username, users, &userIndex); //inserting newly accepted client socked fd in list

        User args; //struct to pass multiple arguments to server function
        args.port = ClientFD;
        strcpy(args.username, username);

        pthread_create(&thr, NULL, clientHandler, (void *)&args); //creating thread for every client connected
        pthread_detach(thr);
    }
    //close the server
    close(ServerFD);
}

//client handler for each new client
void *clientHandler(void *arguments)
{
    User *args = arguments;
    char clientUsername[10]; 
    strcpy(clientUsername, args->username);
    int clientPort = args->port;

    char *strp; //
    char *massage = (char *)malloc(MAXDATALEN); 
    int x;
    int massageLenght;
    char buffer[MAXDATALEN];//this buffer keeps massage that server sends 

    // Handling messages
    while (1)
    {
        bzero(buffer, MAXDATALEN); //erases the data in 256 byte of buffer started from head of buffer and put zero instead
        massageLenght = recv(clientPort, buffer, MAXDATALEN, 0); //this function receive massage from connected client

        //quit ()
        if (!massageLenght || strncmp(buffer, "/quit", 5) == 0)
        {

            printf("%s(%d) left chat :( \n", clientUsername, clientPort);
            leftGroup(clientPort, users, &userIndex);

            for (int i = 0; i < MAXGROUPS; i++)
            {
                leftGroup(clientPort, groups[i], &group_tail[i]);
            }

            showOnlines(users, userIndex);
            close(clientPort);
            free(massage);
            break;
        }
        else if (strncmp(buffer, "/join", 5) == 0)
        {
            char *group_id_str = malloc(sizeof(MAXDATALEN));
            strcpy(group_id_str, buffer + 6);
            int group_id = atoi(group_id_str);
            printf("%s(port %d) joined in group %d. \n", clientUsername, clientPort, group_id);
            insertMember(clientPort, clientUsername, groups[group_id], &group_tail[group_id]);
        }
        
        else if (strncmp(buffer, "/send", 5) == 0)
        {
            int space_pos = next_space(buffer + 6);
            char *group_id_str = malloc(sizeof(MAXDATALEN));
            strncpy(group_id_str, buffer + 6, space_pos);
            int group_id = atoi(group_id_str);

            if (searchMember(clientPort, groups[group_id], group_tail[group_id]) == -1)
            {
                continue;
            }

            printf("%s %s\n", clientUsername, buffer);
            strcpy(massage, clientUsername);
            x = strlen(massage);
            strp = massage;
            strp += x;
            strcat(strp, buffer + 7 + space_pos);
            massageLenght = strlen(massage);

            for (int i = 0; i < group_tail[group_id]; i++)
            {
                if (groups[group_id][i].port != clientPort)
                    send(groups[group_id][i].port, massage, massageLenght, 0);
            }

            bzero(massage, MAXDATALEN);
        }
        else if (strncmp(buffer, "/leave", 6) == 0)
        {
            char *group_id_str = malloc(sizeof(MAXDATALEN));
            strcpy(group_id_str, buffer + 7);
            int group_id = atoi(group_id_str);
            printf("%s(port %d) left group %d\n",clientUsername ,clientPort, group_id);
            leftGroup(clientPort, groups[group_id], &group_tail[group_id]);
        }
        
        showOnlines(users, userIndex);
    }
    return 0;
}



void insertMember(int port, char *username, User *list, int *tail)
{
    if (searchMember(port, list, *tail) != -1)
    {
        return;
    }
    User *temp;
    temp = malloc(sizeof(User));
    if (temp == NULL)
        printf("Out of space!");
    temp->port = port;
    strcpy(temp->username, username);
    list[(*tail)++] = *temp;
}

int searchMember(int port, User *list, int tail)
{
    for (int i = 0; i < tail; i++)
    {
        if (list[i].port == port)
            return i;
    }
    return -1;
}

void leftGroup(int port, User *list, int *tail)
{
    int ptr = searchMember(port, list, *tail);
    if (ptr == -1)
    {
        return;
    }

    for (int i = ptr; i < *tail - 1; i++)
    {
        list[i] = list[i + 1];
    }
    (*tail)--;
}

void showOnlines(const User *list, int tail)
{
    printf("Current online users:\n");
    if (tail == 0)
    {
        printf("No one is online\n");
        return;
    }

    for (int i = 0; i < tail; i++)
    {
        printf("%d: %s\t", list[i].port, list[i].username);
    }
    printf("\n\n");
}

int next_space(char *str)
{
    int i = 0;
    while (str[i] != '\0')
    {
        if (str[i] == ' ')
        {
            return i;
        }
        i++;
    }
    return -1;
}
void *quit_signal()
{
    printf("server shut down!\n");
    notify_shutdown();
    exit(0);
}

void notify_shutdown()
{
    //that means no available users
    if (userIndex == 0)
    {
        exit(0);
    }
    else
    {
        for (int i = 0; i < userIndex; i++)
        {
            send(users[i].port, "from server: servers shuts down", 18, 0);
        }
    }
      printf("clients disconnected! we're done!\n");
}
