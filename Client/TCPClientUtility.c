
/*
 *	TCPClientUtility.c
 *
 *	Description:
 *				Utility function to setup and establish connection 
 * 				to server from client.
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

 /* Project Includes Goes Here */
 #include "../common.h"
 #include "../CommandTable.h"

 /* Extern Variables */
extern const int NUM_CMDS;
extern CMDTABLE cmdTable[MAX_NUM_CMDS];

 /* Forward References for functions */
 funcPtr ClientTransmitState(void *, void *, void *);
 funcPtr ClientRecieveState(void *, void *, void *);

 int SetupTCPClientSocket(const char *host, const char *port)
 {
 	/* Define criteria for address creation */
 	struct addrinfo addrCriteria;
 	memset(&addrCriteria, 0, sizeof(addrCriteria));		/* Zero out all unused fields */
 	addrCriteria.ai_family = AF_UNSPEC;					/* v4 or v6 is OK */
 	addrCriteria.ai_socktype = SOCK_STREAM;				/* Only streaming sockets are required */
 	addrCriteria.ai_protocol = IPPROTO_TCP;				/* Only TCP Protocols */
 	
 	/* Get the Addresses */
 	struct addrinfo *servAddr;							/* Holder of the return structure from the search query */
 	int rtnVal = getaddrinfo(host, port 				
 					, &addrCriteria, &servAddr);		/* Form list of valid address structures */
 	/* Check for failures */
 	if(rtnVal != SUCCESS)
 	{
 		DieWithUserMessage("getaddrinfo() failed", gai_strerror(rtnVal));
 	}

 	/* Socket Descriptor */
 	int sock = -1;
 	struct addrinfo *addr;
 	/* Parse through the list of addresses */
 	for(addr = servAddr; addr != NULL; addr = addr->ai_next)
 	{
 		/* Attempt socket creation */
 		sock = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
 		if(sock < 0)
 		{
 			/* Attempt Failed retry with next address */
 			continue;
 		}
 		/* Establisht connect with server */
 		if(connect(sock, addr->ai_addr, addr->ai_addrlen) == SUCCESS)
 		{
 			break;
 		}

 		close(sock);
 		sock = -1;
 	}

 	/* Free Address info structure allocated by getaddrinfo */
 	freeaddrinfo(servAddr);
 	return sock;
 }

 funcPtr ClientTransmitState(void *msg, void *argPtr, void *commandArg)
 {
 	PMSG *pmsg = (PMSG *)msg;					/* Reference to network message structure */
 	CLNTARG *clntArgs = (CLNTARG *)argPtr;		/* Reference to argument structure for command */
 	CMDSTRUCT cmdStruct;						/* Generic command structure */
 	CMDARG *cmdArg = (CMDARG *)commandArg;		/* Generic command argument structure */
 	unsigned char msgBuff[MAX_MSG_SIZE];  		/* Buffer for Network packet */
 	FILE *fp = (FILE *)clntArgs->filePtr;		/* Reference to FILE handle for socket */
 	int cmdIdx = (int)clntArgs->cmdIdx;			/* Local copy of command index */
 	int msgSize = 0;							/* Refernce to encoded network msg size */
 	
#ifdef ENABLE_SM_DEBUG
 	printf("CLIENT_STATEMACHINE: ClientTransmitState()\n");
#endif

 	/* Frame Request Packet */
 	if(cmdTable[cmdIdx].frameRequestPacket(pmsg, &cmdStruct, clntArgs->argv, (void *)cmdArg) != RET_SUCC)
 	{
 		PrintUserMessage("ClientTransmitState() failed", "unable to frame request packet");
 		/* Exit from SM Loop */
 		return (funcPtr)NULL;
 	}
 	/* Encode request packet into network stream */
 	msgSize = EncodeNetworkMsg(pmsg, msgBuff, sizeof(PMSG));
 	/* Sent encoded message off to network */
 	if(PutNetworkMsg(fp, msgBuff, msgSize) != SUCCESS)
 	{
 		PrintUserMessage("ClientTransmitState() failed", "unable to send network message");
 		/* Exit from SM Loop */
 		return (funcPtr)NULL;
 	}
 	/* Put SM in RX mode */
 	return (funcPtr)ClientRecieveState;
 }

 funcPtr ClientRecieveState(void *msg, void *argPtr, void *commandArg)
 {
 	PMSG *pmsg = (PMSG *)msg;					/* Reference to network message structure */
 	CLNTARG *clntArgs = (CLNTARG *)argPtr;		/* Reference to argument structure for command */
 	CMDSTRUCT cmdStruct;						/* Generic command structure */
 	CMDARG *cmdArg = (CMDARG *)commandArg;		/* Generic command argument strucuture */
 	unsigned char msgBuff[MAX_MSG_SIZE];  		/* Buffer for Network packet */
 	FILE *fp = (FILE *)clntArgs->filePtr;		/* Reference to FILE handle for socket */
 	int cmdIdx = (int)clntArgs->cmdIdx;			/* Local copy of command index */
 	int msgSize = 0;							/* Refernce to encoded network msg size */

#ifdef ENABLE_SM_DEBUG
 	printf("CLIENT_STATEMACHINE: ClientRecieveState()\n");
#endif

 	/* Get Network message off the network */
 	if((msgSize = GetNetworkMsg(fp, msgBuff, MAX_MSG_SIZE)) == FAILURE)
 	{
 		PrintUserMessage("ClientRecieveState() failed", "unable to recieve network msg");
 		/* Exit from SM Loop */
 		return (funcPtr)NULL;
 	}
 	/* Decode message into messge structure */
 	if(DecodeNetworkMsg(pmsg, msgBuff, msgSize) == FAILURE)
 	{
 		PrintUserMessage("ClientRecieveState() failed", "unable to decode network msg");
 		/* Exit from SM Loop */
 		return (funcPtr)NULL;
 	}

 	/* Check response packet */
 	switch(pmsg->cmdType)
 	{
 		case CMD_RSP:
#ifdef ENABLE_SM_DEBUG
 		printf("CLIENT_STATEMACHINE: CMDTYPE => CMD_RSP\n");
#endif
	 	/* Parse response packet */
	 	if(cmdTable[cmdIdx].parseResponsePacket(pmsg, &cmdStruct, (void *)cmdArg) != RET_SUCC)
	 	{
	 		PrintUserMessage("parseResponsePacket() failed", "cannot parse response packet");
	 		return (funcPtr)NULL;
	 	}
 		break;

 		case CMD_RSP_END:
#ifdef ENABLE_SM_DEBUG
 		printf("CLIENT_STATEMACHINE: CMDTYPE => CMD_RSP_END\n");
#endif
	 	/* Parse response packet */
	 	if(cmdTable[cmdIdx].parseResponsePacket(pmsg, &cmdStruct, (void *)cmdArg) != RET_SUCC)
	 	{
	 		PrintUserMessage("CLIENT_STATEMACHINE: parseResponsePacket() failed", "cannot parse response packet");
	 	}
 		/* Exit from SM Loop */
 		return (funcPtr)NULL;

 		case CMD_NOTFOUND:
 		PrintUserMessage("CLIENT:STATEMACHINE: ClientRecieveState() failed", "command not found");
 		/* Exit from SM Loop */
 		return (funcPtr)NULL;

 		case CMD_PARAM_ERR:
 		PrintUserMessage("CLIENT_STATEMACHINE: ClientRecieveState() failed", "parameter error");
 		/* Exit from SM Loop */
 		return (funcPtr)NULL;

 		default:
 		PrintUserMessage("CLIENT_STATEMACHINE: ClientRecieveState() failed", "unknown msg type");
 		return (funcPtr)NULL;

 	}

 	/* Continue in RX state */
 	return (funcPtr)ClientRecieveState;
 }

 void TCPClientHandler(void *msg, void *clntArgs, void *cmdArg)
 {
 	pFuncPtr ClntSMHandler = (pFuncPtr)ClientTransmitState;

 	while(1)
 	{
 		if(ClntSMHandler == NULL)
 		{
 			/* Exit from SM Loop */
 			return;
 		}
 		ClntSMHandler = (pFuncPtr)ClntSMHandler(msg, clntArgs, (void *)cmdArg);
 	}

 }