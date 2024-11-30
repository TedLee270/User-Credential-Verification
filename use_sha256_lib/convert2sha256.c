#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "sha256_lib.h"

int print=0;  // change it to 1 using  atoi(arg[3]) if needed
int num_items = 1000000; // change it to  ny number using  atoi(arg[4]) if needed
 

int main(int argc, char *argv[]) {
    SHA256_CTX ctx;
    uint8_t hash[SHA256_DIGEST_SIZE];

    FILE *infp, *outfp;
    char line[512], *email, *passwd, *tmp;

    int i;
    uint8_t *data;

    if (argc < 3) {
      fprintf(stderr,"Usage: %s input-text output-sha256.txt [print=0] [item=1M]\n", argv[0]);
      return 1;
    }
    if(argc == 4) print = atoi(argv[3]); // if 4th parm is given for print or no print 
    if(argc == 5) num_items = atoi(argv[4]); // if 5th parm is given for num of items lines 

    infp = fopen(argv[1],"r");
    if(infp==NULL){
      fprintf(stderr,"cannot open input file: %s\n", argv[1]);
      return 1;
    }

    outfp = fopen(argv[2],"w");
    if(outfp==NULL){
      fprintf(stderr,"cannot open output file: %s\n", argv[2]);
      fclose(infp);
      return 1;
    }
    while(num_items && fgets(line, 511, infp) != NULL){
       tmp = strstr(line,"\n");
       if(tmp) *tmp='\0'; // replace newline char with null char

       // printf("I got : %s\n", line);
       if((passwd=strstr(line,":"))== NULL) { // ignore if the line does not have :
           fprintf(stderr,"Ignore: %s\n", line); 
           continue; 
       }
       num_items--;

       email = line; *passwd = '\0'; passwd++;

       if(print) printf("Email : %s\nPasswd: %s\n", email, passwd);

       data = (uint8_t *) email;
       sha256_init(&ctx);
       sha256_update(&ctx, data, strlen((char *)data));
       sha256_final(&ctx, hash);

       if(print) printf("Input email for SHA-256 Hash: %s\n", data);
       if(print) printf("SHA-256 Hash: ");
       for (i = 0; i < SHA256_DIGEST_SIZE; ++i) {
          fprintf(outfp,"%02x", hash[i]);
          if(print) printf("%02x", hash[i]);
       }
       fprintf(outfp, ":");
       if(print) printf(":");

       data = (uint8_t *) passwd;
       sha256_init(&ctx);
       sha256_update(&ctx, data, strlen((char *)data));
       sha256_final(&ctx, hash);

       if(print) printf("Input passwd for SHA-256 Hash: %s\n", data);
       if(print) printf("SHA-256 Hash: ");
       for (i = 0; i < SHA256_DIGEST_SIZE; ++i) {
          fprintf(outfp,"%02x", hash[i]);
          if(print) printf("%02x", hash[i]);
       }
       fprintf(outfp, "\n");
       if(print) printf("\n");
   }
   fclose(outfp);
   fclose(infp);
   return 0;
}

