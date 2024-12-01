/* client.c - Modified code for assignment */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>  // Include for gethostbyname()
#include <unistd.h>

// Including the header file for the struct
#include "sha256_lib.h"


int clientSocket;    /* socket descriptor */ 
int port;            /* protocol port number */  

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

int main(int argc, char *argv[])
{ 
  struct sockaddr_in sad; /* structure to hold an IP address     */
  struct hostent  *ptrh;  /* pointer to a host table entry       */
  char *host;           /* pointer to host name                */
  
  char Sentence[128]; 
  char modifiedSentence[128]; 
  char buff[128];
  int n;
  
  if (argc != 3) {
    fprintf(stderr,"Usage: %s server-name port-number\n",argv[0]);
    exit(1);
  }
  
  /* Extract host-name from command-line argument */
  host = argv[1];         /* if host argument specified   */
  
  /* Extract port number  from command-line argument */
  port = atoi(argv[2]);   /* convert to binary            */
  
  /* Create a socket. */
  
  clientSocket = socket(PF_INET, SOCK_STREAM, 0);
  if (clientSocket < 0) {
    fprintf(stderr, "socket creation failed\n");
    exit(1);
  }

  /* Connect the socket to the specified server. */

  memset((char *)&sad,0,sizeof(sad)); /* clear sockaddr structure */
  sad.sin_family = AF_INET;           /* set family to Internet     */
  sad.sin_port = htons((u_short)port);
  ptrh = gethostbyname(host); /* Convert host name to equivalent IP address and copy to sad. */
  if ( ((char *)ptrh) == NULL ) {
    fprintf(stderr,"invalid host: %s\n", host);
    exit(1);
  }
  memcpy(&sad.sin_addr, ptrh->h_addr, ptrh->h_length);
  
  if (connect(clientSocket, (struct sockaddr *)&sad, sizeof(sad)) < 0) {
    fprintf(stderr,"connect failed\n");
    exit(1);
  }
  
  // Main loop for the client
  while(1) {
    char choice;
    char user_email[128];
    char user_password[128];
    char hashed_user_email[SHA256_BLOCK_SIZE * 2 + 1];
    char hashed_password[SHA256_BLOCK_SIZE * 2 + 1];
    clock_t start, stop;
    double elapsed_time;

    printf("Enter an option:\n");
    printf("1: Check username or email\n");
    printf("2: Check password\n");
    printf("3: Check both username/email and password\n");
    printf("4: Exit\n");
    scanf(" %c", &choice);

    switch (choice) { // Change to if statements too
      case '1':
        printf("Enter your username or email: ");
        scanf("%s", user_email);

        start = clock(); // Begin timer
        sha256_hash(user_email, hashed_user_email);
        stop = clock(); // End timer

        // Calculate elapsed time
        elapsed_time = ((double)(stop - start)) / CLOCKS_PER_SEC;
        
        // Send the hashed email to the server
        send(clientSocket, hashed_user_email, strlen(hashed_user_email), 0);

        // Receive the response from the server
        n = recv(clientSocket, buff, sizeof(buff), 0);
        buff[n] = '\0'; // Null-terminate the received data
        printf("Server reply: %s\n", buff);
        printf("Elapsed time: %lf seconds\n", elapsed_time);
        break;
      case '2':
        printf("Enter your password: ");
        scanf(" %s", user_password);

        start = clock(); // Begin timer
        sha256_hash(user_password, hashed_password);
        stop = clock(); // End timer

        // Calculate elapsed time
        elapsed_time = ((double)(stop - start)) / CLOCKS_PER_SEC;

        // Send the hashed password to the server
        send(clientSocket, hashed_password, strlen(hashed_password), 0);

        // Receive the response from the server
        n = recv(clientSocket, buff, sizeof(buff), 0);
        buff[n] = '\0'; // Null-terminate the received data
        printf("Server reply: %s\n", buff);
        printf("Elapsed time: %lf seconds\n", elapsed_time);
        break;
      case '3':
        printf("Enter your username or email: ");
        scanf("%s", user_email);
        sha256_hash(user_email, hashed_user_email);
        
        printf("Enter your password: ");
        scanf(" %s", user_password);
        sha256_hash(user_password, hashed_password);

        start = clock(); // Begin timer
        // Sending both the hashed username/email and password to the server
        send(clientSocket, hashed_user_email, strlen(hashed_user_email), 0);
        send(clientSocket, hashed_password, strlen(hashed_password), 0);
        stop = clock(); // End timer

        // Calculate elapsed time
        elapsed_time = ((double)(stop - start)) / CLOCKS_PER_SEC;

        // Receive the response from the server
        n = recv(clientSocket, buff, sizeof(buff), 0);
        buff[n] = '\0'; // Null-terminate the received data
        printf("Server reply: %s\n", buff);
        printf("Elapsed time: %lf seconds\n", elapsed_time);
        break;
      case '4':
        printf("Thank you for using the program! Now exiting...\n");
        send(clientSocket, "exit", strlen("exit"), 0); // Sending exit message
        close(clientSocket);
        return 0;
      default:
        printf("Not a valid option. Please try again.\n");
        break;
    }
  }
  
  /* Close the socket. */
  close(clientSocket);
  return 0;  
}
