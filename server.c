/*
Computer Networks: Programming Assignment 1
Gabriel Corella_Xing Gao
*/    

#include <stdio.h>         
#include <sys/socket.h>    
#include <stdlib.h>        
#include <netinet/in.h>    
#include <string.h>        
#include <unistd.h>        
#include <netdb.h>   


/* SERVER_PORT is the port number on which the server listens for
   incoming requests from clients. You should change this to a different
   number to prevent conflicts with others in the class. */

// User able to pick a port: 49152 through 65535
#define SERVER_PORT 49152
#define CAPACITY 2048   
#define SIZE 80
#define LOCALHOST "127.0.0.1"

int byte_total;

short seqNo;
short count;

char message[4];

int sock_server;  // Socket on which server listens to clients */
int server_socket;  // Socket on which server exchanges data with client */

struct sockaddr_in server_addr;  // Server Address Struct
struct sockaddr_in client_addr;  

unsigned int client_addr_len;  // Client Address Len
unsigned int server_addr_len;  // Server Address Len

FILE *fileOutput = NULL; // File to output to : 'out.txt'
short decodeHeader(char* packetData, int begin, int end);

int main(void) {
  // open a socket 
  if ((sock_server = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
    perror("Server: can't open stream socket");
    exit(1);                                                
  }
  
  // Server Address intialize and 
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_addr.s_addr = htonl (INADDR_ANY);  
  server_addr.sin_family = AF_INET;
  unsigned short server_port = SERVER_PORT; 
  server_addr.sin_port = htons(SERVER_PORT);

  // bind socket to the local server port 
  if (bind(sock_server, (struct sockaddr *) &server_addr,sizeof (server_addr)) < 0) {
    perror("Error: Could not bind connection. Make clean, and restart connection.\n");
    close(sock_server);
    exit(1);
  }                     

  if (listen(sock_server, 50) < 0) {   // Max queuing requests is 50 by default  
    perror("Error: Listening error occurred."); 
    close(sock_server);
    exit(1);
  }
  printf("Listening on port: %hu\n\n", server_port);
  client_addr_len = sizeof (client_addr);

  /*
   Extract the first connection on the queue of pending connections, 
   create a new socket with the same socket type protocol and address family as 
   the specified socket
  */

  server_socket = accept(sock_server, (struct sockaddr *) &client_addr, &client_addr_len);
  if (server_socket < 0) {
    perror("Error: Server unable to accept() connection \n"); 
    close(sock_server);
    exit(1);
  } else {
    printf("Connected on Port: %hu \n\n", server_port);
  }
  int recvBytes = recv(server_socket, message, 4, 0);
  
  count = decodeHeader(message, 0, 1);
  seqNo = decodeHeader(message, 2, 3);
  
  //receive filename data
  char filename[count+1];  
  recvBytes = recv(server_socket, filename, sizeof(filename)-1, 0);

  // Null byte (count + 1)
  filename[count+1] = '\0';
  if (recvBytes > 0){
    printf("File Requested: %s\n", filename);
    fileOutput = fopen(filename, "r");
    seqNo = 1;
    byte_total = 0;
    
    
    if(fileOutput){
      char *buff = calloc(1,SIZE);
      // Dont forget to free mem
      
      while(fgets(buff,SIZE,fileOutput) != NULL){
        int buffLen = strlen(buff);

        uint16_t packetHeader[2];
        // Decode packet header
        packetHeader[1] = htons(seqNo);
        packetHeader[0] = htons(buffLen);
   
        int sentBytes_1 = send(server_socket, packetHeader, 4, 0);
        int sentBytes_2 = send(server_socket, buff, buffLen, 0);
        
        // Implement Packet Loss function 
        printf("Packet %d transmitted with %d data bytes\n", seqNo, sentBytes_2);
        byte_total += sentBytes_2;
        seqNo++;
        memset(buff, 0, SIZE);
      }
      free(buff);
      fclose(fileOutput);
    }
    else{
      //send file not found message packet
      printf("File name not found... please make sure file is in local directory \n");
    }
    printf("*********************************** \n");
    printf("\n");
    printf("Number of data packets transmitted: %d\n", seqNo-1);
    // sum of the count fields of all reciveced packets
    printf("Total number of data bytes transmitted: %d\n", byte_total);

    //close socket and quit
    close(server_socket); 
    exit(0);
  }
  close(server_socket); 
  return (1);
}
short decodeHeader(char* packetData, int begin, int end){
  // 16 bit int
  uint16_t num;
  char *header = (char*)&num;
  header[0] = packetData[begin];
  header[1] = packetData[end];
  short retNum = ntohs(num);
  return retNum;
}
