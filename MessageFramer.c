/*
 *	MessageFramer.c
 * 
 *	Author		:	Antony Clince Alex
 *	Date		:	08/11/2013
 *	Description	:	Contains message framing implementation
 */

 /* Standard includes goes here */
 #include <stdio.h>
 #include <string.h>

 /* Porject includes goes here */
 #include "common.h"
 #include "MsgProtocol.h"

 int GetNetworkMsg(FILE *in, unsigned char *out_buf, size_t bufSize)
 {
 	int msgSize = 0;
 	size_t retVal = 0;
#ifdef ENABLE_RX_DUMP
 	int iVal = 0;
#endif
 	
 	/* Read the message size from network stream(first four bytes) */
 	if((retVal = fread(&msgSize, sizeof(msgSize), 1, in)) != 1)
 	{
 		fprintf(stderr, "Framing Error: expected %d, but read %d\n", (int)sizeof(msgSize), (int)retVal);
 		return FAILURE;
 	}
 	/* Change byte ordering from n/w to host */
 	msgSize = ntohl(msgSize);
 	/* Check for Buffer Overflow */
 	if(msgSize > bufSize)
 	{
 		fprintf(stderr, "Framing Error: max message size %d, but recieved %d\n", (int)bufSize, (int)msgSize);
 		return FAILURE;
 	}

#ifdef ENABLE_PKTSIZE_DEBUG
 	printf("Network Message Size(read) = %d\n", (int)msgSize);
#endif

 	/* Read rest of the data off the network stream */
 	if(fread(out_buf, sizeof(unsigned char), msgSize, in) != msgSize)
 	{
 		fprintf(stderr, "Framing Error: expected %d, but read less\n", (int)msgSize);
 		return FAILURE;
 	}

#ifdef ENABLE_PKTSIZE_DEBUG
 	printf("Read %d bytes to network\n", msgSize);
#endif

#ifdef ENABLE_RX_DUMP
 	 printf("--------------RX DUMP--------------\n");
 	for(iVal = 0; iVal < msgSize; iVal++)
 	{
 		if((iVal % 8) == 0)
 		{
 			printf("\n");
 		}
 		printf("0x%.2X ", out_buf[iVal]);
 	}
 	printf("\n");
 	printf("-----------------------------------\n");
#endif

 	/* Returns the read message size */
 	return msgSize;
 }

 int PutNetworkMsg(FILE *out, unsigned char *in_buf, size_t bufSize)
 {
 	int retVal = 0;
 	int msgSize;
#ifdef ENABLE_TX_DUMP
 	int iVal = 0;
#endif

 	/* Check for TX Buffer Overflow */
 	if(bufSize > MAX_MSG_SIZE)
 	{
 		fprintf(stderr, "Framing Error: max buffer size %d, but recieved %d\n", MAX_MSG_SIZE, (int)bufSize);
 		return FAILURE;
 	}
 	/* Copy the message size */
 	msgSize = (int)bufSize;
 	/* Convert to network byte ordering */
 	msgSize = htonl(msgSize);
 	/* Write message size to stream */
 	if(fwrite(&msgSize, sizeof(int), 1, out) != 1)
 	{
 		fprintf(stderr, "Framing Error: expected %d, but wrote less\n", (int)sizeof(msgSize));
 		return FAILURE;
 	}

#ifdef ENABLE_PKTSIZE_DEBUG
 	printf("Network Message Size(write) = %d\n", (int)bufSize);
#endif

 	/* Write rest of the message to stream */
 	if((retVal = fwrite(in_buf, sizeof(unsigned char), bufSize, out)) != bufSize)
 	{
 		fprintf(stderr, "Framing Error: expected %d, but wrote only %d\n", (int)bufSize, retVal);
 		return FAILURE;
 	}

#ifdef ENABLE_PKTSIZE_DEBUG
 	printf("Wrote %d bytes to network\n", (int)bufSize);
#endif

#ifdef ENABLE_TX_DUMP
 	printf("--------------TX DUMP--------------\n");
 	for(iVal = 0; iVal < bufSize; iVal++)
 	{
 		if((iVal % 8) == 0)
 		{
 			printf("\n");
 		}
 		printf("0x%.2X ", in_buf[iVal]);
 	}
 	printf("\n");
 	printf("-----------------------------------\n");
#endif


 	/* Flush the network stream */
 	fflush(out);
 	return SUCCESS;
 }