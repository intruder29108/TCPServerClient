# Makefile to Build TCP Client

# Setup Compiler
CC=gcc
# Setup Compiler Flags
CFLAGS = -pthread -fno-stack-protector
# Setup Build Environment
FILE_PATH	=	../

all : TCPClient

TCPClient : TCPClient.o TCPClientUtility.o $(FILE_PATH)CommandTable.o $(FILE_PATH)DieWithMessage.o  $(FILE_PATH)ProgressBar.o $(FILE_PATH)AddressUtility.o $(FILE_PATH)MessageFramer.o $(FILE_PATH)MessageEncoder.o $(FILE_PATH)FileUtility.o
	$(CC) $(CFLAGS) TCPClient.o TCPClientUtility.o $(FILE_PATH)ProgressBar.o $(FILE_PATH)CommandTable.o $(FILE_PATH)DieWithMessage.o $(FILE_PATH)AddressUtility.o $(FILE_PATH)MessageFramer.o $(FILE_PATH)MessageEncoder.o $(FILE_PATH)FileUtility.o -o TCPClient	 

clean : 
	rm -rf ${FILE_PATH}*.o *.o TCPClient