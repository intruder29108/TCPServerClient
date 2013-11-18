# Makefile to Build TCP Server

# Setup Compiler
CC=gcc
# Setup Compiler Flags
CFLAGS = -pthread -fno-stack-protector
# Setup Build Environment
FILE_PATH	=	../

all : TCPServer

TCPServer : TCPServer.o TCPServerUtility.o $(FILE_PATH)CommandTable.o $(FILE_PATH)ProgressBar.o $(FILE_PATH)DieWithMessage.o $(FILE_PATH)AddressUtility.o $(FILE_PATH)MessageFramer.o $(FILE_PATH)MessageEncoder.o $(FILE_PATH)FileUtility.o
	$(CC) $(CFLAGS) TCPServer.o TCPServerUtility.o $(FILE_PATH)ProgressBar.o $(FILE_PATH)CommandTable.o $(FILE_PATH)DieWithMessage.o $(FILE_PATH)AddressUtility.o $(FILE_PATH)MessageFramer.o $(FILE_PATH)MessageEncoder.o $(FILE_PATH)FileUtility.o -o TCPServer	 

clean : 
	rm -rf ${FILE_PATH}*.o *.o TCPServer	