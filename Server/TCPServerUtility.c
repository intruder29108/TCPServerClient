/*
 *	SetupTCPServerSocket.c
 *
 *	Description:
 *				Utility functions to setup TCP Server
 * 				
 *
 *	Author	:	Antony Clince Alex
 *	Date	: 	07/11/2013
 */

 /* Standard Includes Goes here */
 #include <string.h>
 #include <unistd.h>
 #include <sys/types.h>
 #include <sys/socket.h>
 #include <netdb.h>
 #include <stdlib.h>

 /* Project Includes Goes Here */
 #include "../common.h"
 #include "../MsgProtocol.h"
 #include "../MessageFramer.h"
 #include "../MessageEncoder.h"
 #include "../CommandTable.h"

static const int MAXPENDING = 5;					/* Maximum outstanding connection request */

/* Extern Variables */
extern const int NUM_CMDS;
extern CMDTABLE cmdTable[MAX_NUM_CMDS];

/* Forward referencing Functions */
funcPtr ServerRecieveState(void *, void *, void *);
funcPtr ServerTramsmitState(void *, void *, void *);

int SetupTCPServerSocket(const char *service)
{
	/* Construct the server address structure */
	struct addrinfo addrCriteria;
	memset(&addrCriteria, 0, sizeof(addrCriteria));	/* Zero out all the unused fields */
	addrCriteria.ai_family = AF_UNSPEC;				/* Accept both v4 and v6 */
	addrCriteria.ai_socktype = SOCK_STREAM;			/* Only Stream type */
	addrCriteria.ai_flags = AI_PASSIVE;				/* Accept on any address */
	addrCriteria.ai_protocol = IPPROTO_TCP;			/* Accept only TCP protocol */
	
	struct addrinfo *servAddr;						/* List of server addresses */
	int rtnVal = getaddrinfo(NULL, service, &addrCriteria, &servAddr);
	if(rtnVal != SUCCESS)
	{
		DieWithUserMessage("getaddrinfo() failed", gai_strerror(rtnVal));
	}	
	int servSock = -1;								/* Initialize descriptor for server socket */
	struct addrinfo *addr;
	for(addr = servAddr; addr != NULL; addr = addr->ai_next)
	{
		servSock = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
		if(servSock < 0)
		{
			/* Socket creation failed, retry with next entry */
			continue;
		}
		/* Bind to local address and set socket to listen */
		if((bind(servSock, addr->ai_addr, addr->ai_addrlen) == SUCCESS) &&
				(listen(servSock, MAXPENDING) == SUCCESS))
		{
			/* Print the local address of the socket */
			struct sockaddr_storage localAddr;
			socklen_t addrSize = sizeof(localAddr);
			if(getsockname(servSock, (struct sockaddr *)&localAddr, &addrSize) < 0)
			{
				DieWithSystemMessage("getsockname() failed");
			}
			fputs("Binding to ", stdout);
			PrintSocketAddress((struct sockaddr *)&localAddr, stdout);
			fputc(NEW_LINE, stdout);
			break;
		}
		close(servSock);
		servSock = -1;
	}

	/* Free address list allocated by getaddrinfo() */
	freeaddrinfo(servAddr);

	return servSock;
}

int AcceptTCPConnection(int servSock)
{
	struct sockaddr_storage clntAddr;			/* Cliend address structure */
	socklen_t clntAddrLen = sizeof(clntAddr);	/* Store size of structure */
	
	/* Wait for client to connect */
	int clntSock = accept(servSock, (struct sockaddr *)&clntAddr, &clntAddrLen);
	if(clntSock < 0)
	{
		DieWithSystemMessage("accept() failed");
	}
	/* Client is connected */
	fputs("Handling client ", stdout);
	PrintSocketAddress((struct sockaddr *)&clntAddr, stdout);
	fputc(NEW_LINE, stdout);

	return clntSock;
}

void HandleTCPClient(int clntSocket)
{
	/* Allocate D.S for each thread */
	PMSG pmsg;
    FILE *fp = NULL;
    CMDARG commandArg;

#ifdef USE_PTHREAD
    pFuncPtr SMHandler = (pFuncPtr)ServerRecieveState;				/* Initialize SM Handler to RX State */
#endif
    fp = fdopen(clntSocket, "r+");									/* File handle to socket stream */

#ifdef DEBUG_INCOMING_CON
    printf("Created client socket with id => %d\n", clntSocket);
#endif
	
#ifdef USE_PTHREAD
    while(1)
    {
    	SMHandler = (pFuncPtr)SMHandler((void *)&pmsg, (void *)fp, (void *)&commandArg);

    	/* Check for NULL Pointer */
    	if(SMHandler == NULL)
    	{
        	break;
    	}
    }

#endif

	/* Close file handle for Client Socket */
	fclose(fp);
	return;
}

funcPtr ServerRecieveState(void *msg, void *filePtr, void *commandArg)
{
	PMSG *pmsg = (PMSG *)msg;								/* Reference to Message structure */
	FILE *fp  = (FILE *)filePtr;							/* Reference Socket File handle */
	CMDARG *cmdArg = (CMDARG *)commandArg;					/* Generic Command Argument */
	int recvMsgSize = 0;									/* Message Size Copy */
	unsigned char msgBuff[MAX_MSG_SIZE];					/* Hold encoded network message */

#ifdef ENABLE_SM_DEBUG
	printf("SERVER STATEMACHINE => ServerRecieveState()\n");
#endif
	/* Read request packet off the network */
    if((recvMsgSize = GetNetworkMsg(fp, msgBuff, MAX_MSG_SIZE)) == FAILURE)
    {
    	PrintUserMessage("GetNetworkMsg() failed", "unable to recieve");
		/* Exit from SM Loop */
		return (funcPtr)NULL;
    }
    /* Decode the network packet into Network Message Structure */
    if(DecodeNetworkMsg(pmsg, msgBuff, recvMsgSize) == FAILURE)
    {
    	PrintUserMessage("DecodeNetworkMsg() failed", "unable to decode");
    	/* Exit from SM Loop */
    	return (funcPtr)NULL;
    }
	/* Move to TX State */
	return (funcPtr)ServerTramsmitState;
}

funcPtr ServerTramsmitState(void *msg, void *filePtr, void *commandArg)
{
	int iVal = 0;									/* Local count value */
	PMSG *pmsg = (PMSG *)msg;						/* Reference to network message */
	FILE *fp  = (FILE *)filePtr;					/* Reference to Socket File handle */
	CMDARG *cmdArg = (CMDARG *)commandArg;			/* Generic Command Argument structure */
	BOOL cmdFound = FALSE;							/* Local Flag */
	CMDSTRUCT cmdStruct;							/* Generic command structure */
	int sendMsgSize = 0;							/* Local copy to TX MSG Size */
	unsigned char msgBuff[MAX_MSG_SIZE];			/* Hold encoded network message */

#ifdef ENABLE_SM_DEBUG
	printf("SERVER STATEMACHINE: ServerTramsmitState()\n");
#endif

	/* Check command type */
	switch(pmsg->cmdType)
	{
		case CMD_REQ:
#ifdef ENABLE_SM_DEBUG
		printf("SERVER STATEMACHINE: CMDTYPE => CMD_REQ\n");
#endif
		/* Reset command index */
		cmdArg->cmdIdx = -1;
		/* Parse through entire command table */
		for(iVal = 0; iVal < NUM_CMDS; iVal++)
		{
			if(cmdTable[iVal].cmdCode == pmsg->cmdCode)
			{	
				cmdArg->cmdIdx = iVal;		/* Update command index */
				cmdFound = TRUE;
				break;
			}
		}
		if(cmdFound == FALSE)
		{
#ifdef ENABLE_SM_DEBUG
			printf("SERVER STATEMACHINE: command not found\n");
#endif
			/* Frame Command not found response */
			frameErrorResponse(pmsg, CMD_NOTFOUND);
			/* Encode  Network packet into byte stream */
			sendMsgSize = EncodeNetworkMsg(pmsg, msgBuff, sizeof(PMSG));
			/* Send byte stream into network */
			if(PutNetworkMsg(fp, msgBuff, sendMsgSize) != SUCCESS)
			{
				PrintUserMessage("PutNetworkMsg() failed", "unable to send");
				/* Reset command index */
				cmdArg->cmdIdx = -1;
    			/* Exit from SM Loop */
				return (funcPtr)NULL;
			}
			/* Reset command index */
			cmdArg->cmdIdx = -1;
			/* Exit from SM Loop */
			return (funcPtr)NULL;
		}
		else
		{
		    /* Parse the request */
			if(cmdTable[cmdArg->cmdIdx].parseRequestPacket(pmsg, (void *)&cmdStruct, (void *)cmdArg) != RET_SUCC)
			{
				PrintSystemMessage("parseRequestPacket() failed");
				/* Reset command index */
				cmdArg->cmdIdx = -1;
				/* Exit from SM Loop */
				return (funcPtr)NULL;

			}
			/* Frame first response packet */
			if(cmdTable[cmdArg->cmdIdx].frameResponsePacket(pmsg, (void *)&cmdStruct, NULL, (void *)cmdArg) != RET_SUCC)
			{
				PrintSystemMessage("frameResponsePacket() failed");
				/* Reset command index */
				cmdArg->cmdIdx = -1;
				/* Exit from SM Loop */
				return (funcPtr)NULL;
			}
			/* Encode Into Network packet to byte stream */
			sendMsgSize = EncodeNetworkMsg(pmsg, msgBuff, sizeof(PMSG));
			/* Send byte stream into network */
			if(PutNetworkMsg(fp, msgBuff, sendMsgSize) != SUCCESS)
			{
				PrintUserMessage("PutNetworkMsg() failed", "unable to send");
				/* Reset command index */
				cmdArg->cmdIdx = -1;
				/* Exit from SM Loop */
				return (funcPtr)NULL;
			}
		}
		break;

		case CMD_RSP:
#ifdef ENABLE_SM_DEBUG
		printf("SERVER STATEMACHINE: CMDTYPE => CMD_RSP\n");
#endif
		/* Parse the previous response into network structure */
		if(cmdTable[cmdArg->cmdIdx].parseRequestPacket(pmsg, (void *)&cmdStruct, (void *)cmdArg) != RET_SUCC)
		{
			PrintSystemMessage("parseRequestPacket() failed");
			/* Reset command index */
			cmdArg->cmdIdx = -1;
			/* Exit from SM Loop */
			return (funcPtr)NULL;		
		}
		/* Frame the response packet */
		if(cmdTable[cmdArg->cmdIdx].frameResponsePacket(pmsg, (void *)&cmdStruct, NULL, (void *)cmdArg) != RET_SUCC)
		{
			PrintSystemMessage("frameResponsePacket() failed");
			/* Reset command index */
			cmdArg->cmdIdx = -1;
			/* Exit from SM Loop */
			return (funcPtr)NULL;		
		}
		/* Encode Into Network packet to byte stream */
		sendMsgSize = EncodeNetworkMsg(pmsg, msgBuff, sizeof(PMSG));
		/* Send byte stream off to network */
		if(PutNetworkMsg(fp, msgBuff, sendMsgSize) != SUCCESS)
		{
			PrintUserMessage("PutNetworkMsg() failed", "unable to send");
			/* Reset command index */
			cmdArg->cmdIdx = -1;
			/* Exit from SM Loop */
			return (funcPtr)NULL;
		}
		break;

		case CMD_RSP_END:
#ifdef ENABLE_SM_DEBUG
		printf("SERVER STATEMACHINE: CMDTYPE => CMD_RSP_END\n");
#endif
		/* Reset command index */
		cmdArg->cmdIdx = -1;
		/* Response to request finished terminate SM */
		return (funcPtr)NULL;
		break;

		case CMD_NOTFOUND:
#ifdef ENABLE_SM_DEBUG
		printf("SERVER STATEMACHINE: CMDTYPE => CMD_NOTFOUND\n");
#endif
		/* Reset command index */
		cmdArg->cmdIdx = -1;
		/* Exit from SM Loop */
		return (funcPtr)NULL;

		case CMD_PARAM_ERR:
#ifdef ENABLE_SM_DEBUG
		printf("SERVER STATEMACHINE: CMDTYPE => CMD_PARAM_ERR\n");
#endif
		/* Reset command index */
		cmdArg->cmdIdx = -1;
		/* Exit from SM Loop */
		return (funcPtr)NULL;

		default:
#ifdef ENABLE_SM_DEBUG
		printf("SERVER STATEMACHINE: CMDTYPE => UKNOWN_CMD\n");
#endif
		/* Reset command index */
		cmdArg->cmdIdx = -1;
		/* Exit from SM Loop */
		return (funcPtr)NULL;
	}

	/* Continue in TX State */
	return (funcPtr)ServerTramsmitState;
}

