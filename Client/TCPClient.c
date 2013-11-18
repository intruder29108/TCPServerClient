
/*
 *	TCPEchoClient_V2.c
 *
 *	Description:
 *				Simple Implementation of Echo client using TCP
 * 				Protocol, support but v6 and v4
 *
 *	Author	:	Antony Clince Alex
 *	Date	: 	07/11/2013
 */	

 /* Standard includes goes here */
 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include <unistd.h>
 #include <sys/types.h>
 #include <sys/socket.h>
 #include <netdb.h>

 /* Project includes goes here */
 #include "../common.h"
 #include "../MsgProtocol.h"
 #include "../MessageEncoder.h"
 #include "../MessageFramer.h"
 #include "../CommandTable.h"

 /* Forward referencing Functions */
funcPtr ClientRecieveState(void *, void *);
funcPtr ClientTransmitState(void *, void *);

 int main(int argc, char *argv[])
 {
 	if(argc < 4 || argc > 5)
 	{
 		DieWithUserMessage("Parameter(s)", "<Server-Address/Name> <Cmd Idx> <Echo Word> [<Server Port/Service>]");
 	}

 	char *server = argv[1];					/* First argument : Server address */
 	char *echoString = argv[3];				/* Second argument : Echo string */
 	char *port = argv[4];					/* Third argument (Optional): port/service number */
 	FILE *fp;
 	static CLNTARG clntArgs;
 	static PMSG pmsg;
 	static CMDARG cmdArg;
 	static char cmdArgs[MAX_NUM_PARAMS][MAX_PARAM_BUFFER_SIZE];
  	
  	/* Copy Echo string */
  	strcpy(cmdArgs[0], argv[3]);

 	/* Create a connect TCP socket */
 	int sock = SetupTCPClientSocket(server, port);
 	if(sock < 0)
 	{
 		DieWithUserMessage("SetupTCPClientSocket() failed", "unable to connect");
 	}

 	/* Open file stream using socket */
 	fp = fdopen(sock, "r+");
 	if(fp == NULL)
 	{
 		DieWithSystemMessage("fopen: NULL Pointer");
 	}
 

 	clntArgs.cmdIdx = atoi(argv[2]);
 	clntArgs.argv = (char (*)[MAX_PARAM_BUFFER_SIZE])cmdArgs;
 	clntArgs.filePtr = fp;

 	/* Invode Client Handler */
 	TCPClientHandler((void *)&pmsg, (void *)&clntArgs, (void *)&cmdArgs);


	/* Close the socket */
 	close(sock);										
 	exit(0);
 }