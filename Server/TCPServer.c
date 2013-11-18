
/*
 *	TCPServer.c
 *
 *	Description:
 *				Implementation for TCP Server
 * 				
 *
 *	Author	:	Antony Clince Alex
 *	Date	: 	08/11/2013
 */


 /* Standard includes goes here */
 #include <stdio.h>
 #include <unistd.h>
 #include <pthread.h>
 #include <stdlib.h>
 #include <error.h>

 /* Porject includes goes here */
 #include "../common.h"

 /* Forward Declaration for client thread */
 void *ClientThread(void *arg);
 /* Client Thread Argument Structure */
 typedef struct _THRDARG
 {
 	int clntSock;			/* Socket Descriptor for client */
 } THRDARG;

 int main(int argc, char *argv[])
 {
 	if(argc != 2)
 	{
 		DieWithUserMessage("Parameter(s)", "<Server Port/Service>");
 	}
 	char *service = argv[1];			/* First argument: local port */
 	/* Create socket for incoming connections */
 	int servSock = SetupTCPServerSocket(service);
 	if(servSock < 0)
 	{
 		DieWithUserMessage("SetupTCPServerSocket() failed", service);
 	}

 	/* Run forever */
 	for(;;)
 	{
 		 /* Accept incoming connections */
 		int clntSock = AcceptTCPConnection(servSock);
#ifdef USE_PTHREAD
		/* Create client thread creation */
 		THRDARG *threadArgs = (THRDARG *)malloc(sizeof(THRDARG));
 		if(threadArgs == NULL)
 		{
 			DieWithSystemMessage("malloc() failed");
 		}
 		threadArgs->clntSock = clntSock;
 		/* Create client thread */
 		pthread_t threadID ;
 		int rtnVal = pthread_create(&threadID, NULL, ClientThread, threadArgs);
 		if(rtnVal != SUCCESS)
 		{
 			DieWithUserMessage("pthread_create() failed", (char *)strerror(rtnVal));
 			printf("with thread %lu\n", (unsigned long int)threadID);
 		}
#else
 		/* Single Threaded handler call */
 		HandleTCPClient(clntSock);
 		close(clntSock);
#endif

	}
 	/* Not reached */
 	return 0;
 }

 void *ClientThread(void *threadArgs)
 {
 	/* Guarantees that thread resources are deallocated upon return */
 	pthread_detach(pthread_self());
 	/* Extract the socket file descriptor from argument */
 	int clntSock = ((THRDARG *)threadArgs)->clntSock;
 	free(threadArgs);

 	HandleTCPClient(clntSock);
 	close(clntSock);

 	return(NULL);
 }