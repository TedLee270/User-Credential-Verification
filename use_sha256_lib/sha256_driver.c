#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "sha256_lib.h"

int main(int argc, char *argv[]) {
    SHA256_CTX ctx;
    uint8_t hash[SHA256_DIGEST_SIZE];
    // uint8_t data1[] = "Hello, world!";
    uint8_t data2[] = "Password123";

    int i;
    uint8_t *data;

    if (argc < 2) data = data2; else data = (uint8_t *) argv[1];

    sha256_init(&ctx);
    sha256_update(&ctx, data, strlen((char *)data));
    sha256_final(&ctx, hash);

    printf("Input for SHA-256 Hash: %s\n", data);
    printf("SHA-256 Hash: ");
    for (i = 0; i < SHA256_DIGEST_SIZE; ++i)
        printf("%02x", hash[i]);
    printf("\n");

    return 0;
}

