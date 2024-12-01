/* server.c - Modified code for assignment */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#include "sha256_lib.h"

#define MAX_CREDENTIALS 1000000
#define MAX_CREDENTIAL_SIZE 128

typedef struct {
  char hashed_user_email[SHA256_BLOCK_SIZE * 2 + 1];
  char hashed_password[SHA256_BLOCK_SIZE * 2 + 1];
} Credentials;

int     welcomeSocket, connectionSocket; /* socket descriptors  */
int     port;            /* protocol port number                */
Credentials credentials[MAX_CREDENTIALS];
int num_credentials = 0;

// Function to hash input using SHA-256
void sha256_hash(const char *input, char *hash) {
  SHA256_CTX ctx;
  uint8_t hash_bytes[SHA256_DIGEST_SIZE];

  sha256_init(&ctx);
  sha256_update(&ctx, (uint8_t *)input, strlen(input));
  sha256_final(&ctx, hash_bytes);
  // Convert the hash bytes to a hexadecimal string
  for (int i = 0; i < SHA256_DIGEST_SIZE; i++) {
    sprintf(hash + (i * 2), "%02x", hash_bytes[i]);
  }
}

void sigint_handler(int sigint) {
  printf("SIGINT signal received. Closing connections and exiting...\n");
  if(connectionSocket != -1) {
    close(connectionSocket);
  }
  if(welcomeSocket != -1) {
    close(welcomeSocket);
  }
  exit(0);
}

// Function to read credentials from file
void readCredentials(const char *filename) {
  FILE *file = fopen(filename, "r");
  if (file == NULL) {
    printf("Error opening file: %s\n", filename);
    exit(1);
  }
  char line[MAX_CREDENTIAL_SIZE];
  while (fgets(line, MAX_CREDENTIAL_SIZE, file) != NULL) {
    char *email = strtok(line, ":");
    char *password = strtok(NULL, ":");
    if (email != NULL && password != NULL) {
      strcpy(credentials[num_credentials].hashed_user_email, email);
      strcpy(credentials[num_credentials].hashed_password, password);
      num_credentials++;
    }
  }
  fclose(file);
}

int main(int argc, char *argv[])
{
  struct  sockaddr_in sad; /* structure to hold server's address  */
  struct  sockaddr_in cad; /* structure to hold client's address  */
  socklen_t      alen;     /* length of address                   */
  char buff[MAX_CREDENTIAL_SIZE];
  int n;
  
  // Check command line arguments
  if(argc != 3) {
    fprintf(stderr, "Usage: %s port-number\n", argv[0]);
    exit(1);
  }

  // Extract port number from command line argument
  port = atoi(argv[1]);
  char* hash_file = argv[2];

  // Set up SIGINT handler
  signal(SIGINT, sigint_handler);
  
  /* Create a socket */
  welcomeSocket = socket(PF_INET, SOCK_STREAM, 0); /* CREATE SOCKET */
  if (welcomeSocket < 0) {
    fprintf(stderr, "socket creation failed\n");
    exit(1);
  }
  
  /* Bind a local address to the socket */
  memset((char *)&sad,0,sizeof(sad)); /* clear sockaddr structure   */
  sad.sin_family = AF_INET;           /* set family to Internet     */
  sad.sin_addr.s_addr = INADDR_ANY;   /* set the local IP address   */
  sad.sin_port = htons((u_short)port);/* set the port number        */ 
  
  if (bind(welcomeSocket, (struct sockaddr *)&sad, sizeof(sad)) < 0) {
    fprintf(stderr,"bind failed\n");
    exit(1);
  }
  
  /* Specify the size of request queue */
  if (listen(welcomeSocket, 10) < 0) {
    fprintf(stderr,"listen failed\n");
    exit(1);
  }
  
  /* Main server loop - accept and handle requests */
  while (1) {
    alen = sizeof(cad);
    if ( (connectionSocket=accept(welcomeSocket, (struct sockaddr *)&cad, &alen)) < 0) {
      fprintf(stderr, "accept failed\n");
      exit(1);
    }
    
    // Receive request from client
    n = recv(connectionSocket, buff, sizeof(buff), 0);
    if(n <= 0) {
      fprintf(stderr, "recv failed\n");
      close(connectionSocket);
      continue;
    }
    buff[n] = '\0';
    printf("Packet receieved: %s\n", buff);

    // Handle client requests
    if(strcmp(buff, "exit") == 0) {
      printf("Client requests to exit server...\n");
      close(connectionSocket);
      continue;
    }
    else {
      int type_request = atoi(buff);
      char* hash_value = strtok(buff, ":");
      if(hash_value == NULL) {
        send(connectionSocket, "Invalid option\n", strlen("Invalid option\n"), 0;
        }
        else {
          int exists = 0;
          for(int i = 0; i < num_credentials; i++) {
            if(type_request == 1) {
              if(strcmp(hash_value, credentials[i].hashed_user_email) == 0) {
                exists = 1;
                break;
              }
            }
            else if(type_request == 2) {
              if(strcmp(hash_value, credentials[i].hashed_password) == 0) {
                exists = 1;
                break;
              }
            }
            else if(type_request == 3) {
              if(strcmp(hash_value, credentials[i].hashed_user_email) == 0 || strcmp(hash_value, credentials[i].hashed_password) == 0)) {
                exists = 1;
                break;
              }
            }
          }
          if(exists) {
            send(connectionSocket, "Hash value already exists\n", strlen("Hash value already exists\n"), 0);
          }
          else {
            send(connectionSocket, "Hash value does not exist\n", strlen("Hash value does not exist\n"), 0);
          }
        }
      }
    close(connectionSocket);
  }
  return 0;
}
