/*
Computer Networks: Programming Assignment 1
Gabriel Corella_Xing Gao
*/    

#include <stdio.h>         
#include <sys/socket.h>    
#include <stdlib.h>        
#include <netinet/in.h>   // hostent
#include <string.h>        
#include <unistd.h>        
#include <netdb.h>         

// User able to pick a port: 49152 through 65535
#define CLIENT_PORT 49152
#define CAPACITY 2048
#define LOCALHOST "127.0.0.1"

struct hostent * server_hp;
struct sockaddr_in server_addr;

char file[CAPACITY];

short decodeHeader(char* packetData, int begin, int end);
short count;
short seqNo;

int client_socket; // Socket for Client id
int byte_total; // Total number of Bytes
int packet; // Packet ID


int main(void) {
  //define all socket variables
  byte_total = 0;
  packet = 0;

  /* open a socket */
  if ((client_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
    perror("Unable to open Socket: ERROR");
    exit(1);
  }
  
  if ((server_hp = gethostbyname(LOCALHOST)) == NULL) {
    perror("Error: Unable to connect to LocalHost,");
    close(client_socket);
    exit(1);
  }

  /* Server Address info and Initialize */
  memcpy((char *)&server_addr.sin_addr, server_hp->h_addr, server_hp->h_length);
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(CLIENT_PORT);

  
  // Connect, else exit and close socket
  if (connect(client_socket, (struct sockaddr *) &server_addr, sizeof (server_addr)) < 0) {
    perror("Error: Could not connect client to server. Exiting.");
    close(client_socket);
    exit(1);
  } else if(connect(client_socket, (struct sockaddr *) &server_addr, sizeof(server_addr)) == 1){
    printf("Connected on Port: %d \n\n", CLIENT_PORT);
  }
  printf("Which file would you like to send? \n");
  scanf("%s", file);
  int msg_len = strlen(file) + 1;
  printf("\n");

  uint16_t headerBuff[2];
  headerBuff[0] = htons(msg_len);
  headerBuff[1] = htons(1);
  int bytes_sent = send(client_socket, headerBuff, 4, 0);
  bytes_sent = send(client_socket, file, msg_len, 0);

  FILE *outputFile = fopen("out.txt", "wb");

  //Receive in a loop until end of transmission received
  while(1){
   
    char replyMsg[4];
    memset(&replyMsg, 0, 4);
    int bytes_recd1 = recv(client_socket, replyMsg, 4, 0);
  
    //decode header
    count = decodeHeader(replyMsg, 0, 1);
    seqNo = decodeHeader(replyMsg, 2, 3);

    //receive data
    char socketReplyMsg[count+1]; //space for null terminator
    memset(&socketReplyMsg,0,count+1); //ensure buffer is blank
    int recvBytes = recv(client_socket, socketReplyMsg, sizeof(socketReplyMsg)-1, 0);
    socketReplyMsg[count+1] = '\0';
    if(recvBytes < 0){
      perror("File data receive error");
      exit(1);
    }

    //Exit loop if end of transmission
    if(count == 0 && recvBytes == 0){
      //printf("End of Transmission Packet with sequence number %d received with %d data bytes\n", sequenceNum, recvBytes);
      printf("*********************************** \n");
      printf("\n");
      printf("Number of data packets received: %d\n", packet);
      printf("Total number of data bytes received: %d\n", byte_total);
      exit(1);
    }
    else{

      if(outputFile){
        // write to output
        fprintf(outputFile, "%s", socketReplyMsg);
      }

      printf("Packet %d received with %d data bytes\n", seqNo, recvBytes);
      packet = seqNo;
      byte_total += recvBytes;
    }
  }
  
  /* close socket */
  close(client_socket);
}
short decodeHeader(char* packetData, int begin, int end){
  uint16_t num;
  char *header = (char*)&num;
  header[0] = packetData[begin];
  header[1] = packetData[end];
  unsigned short retNum = ntohs(num);
  return retNum;
}
