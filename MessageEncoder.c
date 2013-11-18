/*
 *	MessageEncoder.c
 * 
 *	Author		:	Antony Clince Alex
 *	Date		:	08/11/2013
 *	Description	:	Encodes/Decodes message structure to and from
 *					recieved byte stream
 */

 /* Standard includes goes here */
 #include <stdio.h>
 #include <string.h>

 /* Project includes here */
 #include "common.h"
 #include "MsgProtocol.h"

 size_t EncodeNetworkMsg(const PMSG *pmsg, unsigned char *outBuf, size_t msgSize)
 {
 	
 #ifdef ENABLE_ENCODER_DEBUG
 	printf("Adding Magic Header %d bytes\n", (HEADER_SIZE));
 	printf("Adding %d bytes to network byte array\n", (int)(msgSize));
#endif

 	/* Add Magic header */
 	memcpy(outBuf, MAGIC_HEADER, HEADER_SIZE);
 	/* Copy the Message structure to char array */
 	memcpy(outBuf + HEADER_SIZE, (unsigned char *)pmsg, msgSize);

 	/* Return the size of encoded message */
 	return (msgSize + HEADER_SIZE);
 }

 int DecodeNetworkMsg(PMSG *pmsg, unsigned char *inBuf, size_t msgSize)
 {
 	/* Check for Magic Header */
 	if(memcmp(inBuf, MAGIC_HEADER, HEADER_SIZE) != SUCCESS)
 	{
 		/* Magic Header not found */
 		fprintf(stderr, "DecodeNetworkMsg() failed, magic header not found\n");
 		return FAILURE;
 	}

#ifdef ENABLE_ENCODER_DEBUG
 	printf("Magic Header Detected %d bytes\n", HEADER_SIZE);
 	printf("Reading %d bytes to network packet\n", (int)(msgSize - HEADER_SIZE));
#endif

 	/* Copy the char array to Message Structure */
 	memcpy(pmsg, inBuf + HEADER_SIZE, (int)(msgSize - HEADER_SIZE));

 	return SUCCESS;
 }