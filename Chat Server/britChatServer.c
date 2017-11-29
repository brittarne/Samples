#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <strings.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <stdbool.h>

#define MAX_LINE 1028


typedef
struct vector_st{
  char* data;
  char* receivingUID;
  int size;
  int len;
}vector_t;


typedef
struct client_st{
  char* userid;
  char* secret;
  int authenticated;
  int stillWaiting;
  vector_t output;
  int fd;
  int messageWaiting;
}client_t;


int listen_on(unsigned short port, int queue);
int accept_client(int server);
client_t makeBlankClient (int* fd, int* server);
void client_do_socket(client_t* c, fd_set* rd, fd_set* wr);
void client_send_message(client_t* c, client_t clients[], int totalClients);
void parseMessage(char message[], int n, client_t clients[], int totalClients);

int main(int argc, char* argv[]){
  int portNum = atoi(argv[1]);
  int numOfClients = 0;
  int client;
  int server = listen_on(portNum, 1028);
  int maxClient = server;
  struct client_st clients[MAX_LINE];
  if(server != -1){
    fd_set reads;
    FD_ZERO(&reads);
    FD_SET(server, &reads);

    printf("Server is set\n");
    while(0 <= select(maxClient + 1, &reads, NULL, NULL, NULL)){
      /* new user from server */
      if(FD_ISSET(server, &reads)){
        client = accept_client(server);

        if(client > maxClient){
          maxClient = client;
        }
        /* made a blank client struct */
        clients[numOfClients] = makeBlankClient(&client, &server);
        numOfClients++;
      }
      /* checks if any client has written something */
      for(int j = 0; j < numOfClients; j++){
        if(FD_ISSET(clients[j].fd, &reads)){
          client_do_socket(&clients[j], &reads, &reads);
        }
        /* if a whole message has been found send that message */
        if(clients[j].messageWaiting == 1){
          client_send_message(&clients[j], clients, numOfClients);
          clients[j].messageWaiting = 0;
        }
      }
      FD_ZERO(&reads);
      FD_SET(server, &reads);
      for(int i = 0; i < numOfClients; i++){
        if(clients[i].fd != -1){
          FD_SET(clients[i].fd, &reads);
        }
      }
    }
    printf("%s\n", "Out of select loop");
    close(server);
  }else {
    return EXIT_FAILURE;
  }
}

/* creates the server socket */
int listen_on(unsigned short port, int queue){
  int fd = socket(PF_INET, SOCK_STREAM, 0);
  if(fd == -1) {
    fprintf(stderr, "Call to socket failed because (%d) \"%s\"\n", errno, strerror(errno));
    return -1;
  }
  struct sockaddr_in addr;
  bzero(&addr, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = INADDR_ANY;
  addr.sin_port = htons(port);

  if (bind(fd, (struct sockaddr*)&addr, sizeof(addr))) {
    fprintf(stderr, "Call to bind failed because (%d) \"%s\"\n", errno, strerror(errno));
    close (fd);
    return -1;
  }
  if(listen(fd, queue)) {
    fprintf(stderr, "Call to listen failed because (%d) \"%s\"\n", errno, strerror(errno));
    close (fd);
    return -1;
  }
  return fd;
}

/* creates file descriptor for a client */
int accept_client(int server){
  struct sockaddr_in addr;
  socklen_t len = sizeof(addr);
  bzero(&addr, len);
  int client = accept(server, (struct sockaddr*)&addr, &len);
  if(client == -1){
    fprintf(stderr, "Call to accept failed because (%d) \"%s\"\n", errno, strerror(errno));
    return -1;
  }
  fprintf(stderr, "Received a connection from %s:%d\n", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));
  return client;
}

//works with clausons grading server for single client & with my chatServer
client_t makeBlankClient (int* fd, int* server){
  struct client_st c;
  c.fd = *fd;
  c.authenticated = 0;
  c.stillWaiting = 0;
  c.messageWaiting = 0;
  return c;
}

/* reads messages from client, finds partial messages and whole messages */
void client_do_socket(client_t* c, fd_set* rd, fd_set* wr){
  int n;
  char data[MAX_LINE];
  bzero(data, MAX_LINE);

  printf("Doing a client\n");
  n = read(c -> fd, data, MAX_LINE);

  if(n != 0){
    // entirely new user
    if(c -> authenticated == 0 && c -> stillWaiting == 0){
      //this is the case where userid and secret has not been sent
      printf("Not authenticated\n");
      int sizeOfFirst = strlen(data);
      int sizeOfSecond = 0;
      c -> userid = malloc(sizeOfFirst + 1);
      strcpy(c -> userid, data);
      printf("userid: %s\n", c -> userid);
      if(n == sizeOfFirst + 1 || n == sizeOfFirst){
        c -> stillWaiting = 1;
      }
      else{
        char* secondStart = &data[sizeOfFirst + 1];
        sizeOfSecond = strlen(secondStart);
        c -> secret = malloc(sizeOfSecond + 1);
        strcpy(c -> secret, secondStart);
        c -> authenticated = 1;
        printf("secret: %s\n", c -> secret);
        write(c -> fd, "OK\0\0", 4);
      }
    }
    else if(c -> authenticated == 0 && c -> stillWaiting == 1){
      /* this is the case that the userid has been sent but not the secret */
      printf("Not authenticated, but have userid\n");
      int sizeOfFirst = strlen(data);
      int sizeOfSecond = 0;
      if(sizeOfFirst != 0){
        c -> secret = malloc(sizeOfFirst + 1);
        strcpy(c -> secret, data);
        printf("userid: %s\n", c -> userid);
        printf("secret: %s\n", c -> secret);
        // Only sent userid
        write(c -> fd, "OK\0\0", 4);
        c -> stillWaiting = 0;
      }else{
        sizeOfSecond = strlen(&data[sizeOfFirst + 1]);
        if(sizeOfSecond > 0){
          c -> secret = malloc(sizeOfSecond + 1);
          strcpy(c -> secret, &data[sizeOfFirst + 1]);
          printf("userid: %s\n", c -> userid);
          printf("secret: %s\n", c -> secret);
          write(c -> fd, "OK\0\0", 4);
          c -> authenticated = 1;
          c -> stillWaiting = 0;
        }
      }
    }
    else if(c -> authenticated == 1 && c -> stillWaiting == 0){
      //this is a brand new message
      printf("receiving new message from client: %s\n", c -> userid);
      int sizeOfFirst = strlen(data);
      int sizeOfSecond = 0;
      if(sizeOfFirst != 0){
        printf("Directed Message\n");
        c -> output.receivingUID = malloc(sizeOfFirst + 1);
        strcpy(c -> output.receivingUID, data);
      }
      else{
        printf("Broadcast message\n");
        char* broadcast = "BROADCAST";
        c -> output.receivingUID = malloc(10);
        strcpy(c -> output.receivingUID, broadcast);
      }
      printf("receivingUID: %s\n", c -> output.receivingUID);
      if(n == sizeOfFirst + 1 || n == sizeOfFirst){
        c -> stillWaiting = 1;
      }
      else{
        //sent both
        char* secondStart = &data[sizeOfFirst + 1];
        sizeOfSecond = strlen(secondStart);
        int size = strlen(c -> userid) + sizeOfSecond + 2;
        c -> output.data = malloc(size);
        snprintf(c -> output.data, size, "%s,%s", c -> userid, secondStart);
        c -> output.len = size;
        printf("output.data: %s\n", c -> output.data);
        for(int i = 0; i < size; i++){
          if(c -> output.data[i] == ','){
            c -> output.data[i] = '\0';
            i = size;
          }
        }
        c -> messageWaiting = 1;
      }
    }
    else if(c -> authenticated == 1 && c -> stillWaiting == 1){
      //receiving userid has been found, the message just still needs to be gotten
      printf("Receiving second part\n");
      int sizeOfFirst = strlen(data);
      int sizeOfSecond = 0;
      if (sizeOfFirst != 0){
        //nullbyte has been sent in front
        int size = strlen(c -> userid) + sizeOfFirst + 2;
        c -> output.data = malloc(size);
        snprintf(c -> output.data, size, "%s,%s", c -> userid, data);
        c -> output.len = size;
        printf("output.data: %s\n", c -> output.data);
        for(int i = 0; i < size; i++){
          if(c -> output.data[i] == ','){
            c -> output.data[i] = '\0';
            i = size;
          }
        }
        c -> messageWaiting = 1;
        c -> stillWaiting = 0;
      }else{
        sizeOfSecond = strlen(&data[sizeOfFirst + 1]);
        if(sizeOfSecond > 0){
          int size = strlen(c -> userid) + sizeOfSecond + 2;
          c -> output.data = malloc(size);
          snprintf(c -> output.data, size, "%s,%s", c -> userid, &data[sizeOfFirst + 1]);
          printf("output.data: %s\n", c -> output.data);
          for(int i = 0; i < size; i++){
            if(c -> output.data[i] == ','){
              c -> output.data[i] = '\0';
              i = size;
            }
          }
          c -> messageWaiting = 1;
        }
      }
    }
  }
  else{
    printf("Disconnecting\n");
    c -> fd = -1;
  }
}

/* sends messages to clients based on directed or broadcast messages */
void client_send_message(client_t* c, client_t clients[], int totalClients){
  char* receivingUID;
  char* clientUID;
  receivingUID = c -> output.receivingUID;

  if(strcmp(c -> userid, receivingUID) == 0){
    write(c -> fd, "ERROR\0 'No Messages To Self: Behavior May Be Erratic'\0", 55);
  }
  else{
    if(strcmp(receivingUID, "BROADCAST") == 0){
      for(int j = 0; j < totalClients; j++){
        if(strcmp(clients[j].userid, c -> userid) != 0 && clients[j].fd != -1){
          write(clients[j].fd, c -> output.data, c -> output.len);
          printf("%s broadcast MESSAGE SENT\n", c -> output.data);
        }
      }
    }
    else{
      for(int i = 0; i < totalClients; i++){
        clientUID = clients[i].userid;
        printf("** Sending message\n");
        printf("receivingUID: %s\n", receivingUID);
        printf("sendingUID: %s\n", c -> userid);
        printf("clientUID: %s\n", clientUID);

        if(strcmp(clientUID, receivingUID) == 0){
          write(clients[i].fd, c -> output.data, c-> output.len);
          printf("%s  MESSAGE SENT\n", c -> output.data);
        }
      }
    }
  }
  // free(c -> output.data);
  // free(c -> output.receivingUID);
  //bzero(c -> output.data, c -> output.len);
  //bzero(c -> output.receivingUID, MAX_LINE);
}
