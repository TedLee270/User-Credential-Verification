CC = gcc
CFLAGS = -Wall -Wextra
TARGET =  sha256_driver convert2sha256 

all: $(TARGET)

sha256_driver: sha256_driver.c sha256_lib.o 
	$(CC) $(CFLAGS) -o sha256_driver  sha256_driver.c sha256_lib.o  

convert2sha256: convert2sha256.c sha256_lib.o
	$(CC) $(CFLAGS) -o convert2sha256  convert2sha256.c sha256_lib.o  

sha256_lib.o: sha256_lib.c sha256_lib.h
	$(CC) $(CFLAGS) -c sha256_lib.c 
clean:
	rm -f $(TARGET) *.o

