/*
 *	CommandTable.c
 *
 *	Description	:	Implements individual command request/response
 *					routines.
 *
 *	Author		:	Antony Clince Alex
 *	Date		: 	08/11/2013
 */

/* Standard Includes */
#include <string.h>


/* Project Includes goes here */
#include "common.h"
#include "CommandTable.h"
#include "MsgProtocol.h"
#include "FileUtility.h"
#include "ProgressBar.h"


CMDTABLE cmdTable[]=
	{
		{"echo",	CMD_ECHO,	1,	parseEchoRequest,	frameEchoRequest,	frameEchoResponse,	parseEchoResponse},
#ifdef ENABLE_FILE_TRANSFER
		{"ftp",		CMD_FTP ,	2,	parseFileRequest,	frameFileRequest ,	frameFileResponse,	parseFileResponse}
#endif
	};
/* Command Table Size */
const int NUM_CMDS = sizeof(cmdTable)/sizeof(CMDTABLE);

/* Echo Command Implementation */

/* Client Side API */
RET_CODE parseEchoResponse(const PMSG *pmsg, void *cmdStruct, void *cmdArg)
{
	/* Extact Echo packet */
	ECHOPKT *echoPkt = (ECHOPKT *)cmdStruct;

	/* NULL Pointer Check */
	if(pmsg == NULL || cmdStruct == NULL)
	{
		return RET_NULL;
	}
	memcpy(echoPkt, pmsg->cmdBuffer, pmsg->cmdBufLen);
	/* Print echo packet */
	fputs("Recieved: ", stdout);		
	fputs(echoPkt->payload, stdout);
	fputc(NEW_LINE, stdout);

	return RET_SUCC;
}
 
RET_CODE frameEchoRequest(PMSG *pmsg, void *cmdStruct, char (*argv)[MAX_PARAM_BUFFER_SIZE], void *cmdArg)
{
	/* Extract  Echo packet */
	ECHOPKT *echoPkt = (ECHOPKT *)cmdStruct;
	/* NULL Pointer Check */
	if(pmsg == NULL || cmdStruct == NULL || argv == NULL)
	{
		return RET_NULL;
	}
	/* Update Command Buffer size */
	pmsg->cmdBufLen = strlen(argv[0]) + 1;
	/* Update Echo paket */
	memcpy(echoPkt->payload, argv[0], pmsg->cmdBufLen);
	/* Fill the command buffer with Echo packet */
	memcpy(pmsg->cmdBuffer, echoPkt, pmsg->cmdBufLen);
	/* Update other fields in message packet */
	pmsg->cmdType = CMD_REQ;
	pmsg->cmdCode = CMD_ECHO;

	return RET_SUCC;
}

/* Server Side API for Echo Command */
RET_CODE parseEchoRequest(const PMSG *pmsg, void *cmdStruct, void *cmdArg)
{
	/* Extract Echo packet */
	ECHOPKT *echoPkt = (ECHOPKT *)cmdStruct;
	/* NULL Pointer Check */
	if(pmsg == NULL || cmdStruct == NULL)
	{
		return RET_NULL;
	}
	/* Copy CMDBUFFER to ECHO packet */
	memcpy(echoPkt->payload, pmsg->cmdBuffer, pmsg->cmdBufLen);
	return RET_SUCC;
}

RET_CODE frameEchoResponse(PMSG *pmsg, void *cmdStruct, char (*argv)[MAX_PARAM_BUFFER_SIZE], void *cmdArg)
{
	/* Extract Echo Packet */
	ECHOPKT *echoPkt = (ECHOPKT *)cmdStruct;
	/* NULL Pointer Checks */
	if(pmsg == NULL || cmdStruct == NULL)
	{
		return RET_NULL;
	}
	/* Clear Message Packet */
	memset(pmsg, 0, sizeof(PMSG));
	pmsg->cmdType = CMD_RSP_END;
	pmsg->cmdCode = CMD_ECHO;
	pmsg->cmdBufLen = strlen(echoPkt->payload) + 1;
	memcpy(pmsg->cmdBuffer, echoPkt, sizeof(ECHOPKT));

	return RET_SUCC;
}

RET_CODE frameErrorResponse(PMSG *pmsg, int errorCode)
{
	/* Update the message type to corresponding error code */
	pmsg->cmdType = errorCode;
	pmsg->cmdBufLen = 0;
	memset(pmsg->cmdBuffer, 0, MAX_CMDBUF_SIZE);
	return RET_SUCC;
}

/* Server Side API for File Transfer */
RET_CODE parseFileRequest(const PMSG *pmsg, void *cmdStruct, void *cmdArg)
{
	/* Extract File Request Packet */
	FILEPKT *filePkt = (FILEPKT *)cmdStruct;
	/* Extract Arguments for Command */
	FILEARG *fileArgs = (FILEARG *)cmdArg;
	/* NULL Pointer Checks */
	if(pmsg == NULL || cmdStruct == NULL)
	{
		return RET_NULL;
	}
	/* Copy Command buffer to File Packet */
	memcpy(filePkt, pmsg->cmdBuffer, pmsg->cmdBufLen);

#ifdef ENABLE_FILE_TRANSFER_DEBUG
	printf("FILE_TRANSFER: parseFileRequest()\n");
#endif
	return RET_SUCC;
}

RET_CODE frameFileRequest(PMSG *pmsg, void *cmdStruct, char (*argv)[MAX_PARAM_BUFFER_SIZE], void *cmdArg)
{
	/* Extract reference to FILE Packet */
	FILEPKT *filePkt = (FILEPKT *)cmdStruct;
	/* Extract Arguments for Command */
	FILEARG *fileArgs = (FILEARG *)cmdArg;
	char *fileName;

	/* NULL Pointer Checks */
	if(pmsg == NULL || cmdStruct == NULL|| argv == NULL || fileArgs == NULL)
	{
		return RET_NULL;
	}
	/* Extract Filename */
	fileName = argv[0];

	/* Fill the file packet */
	filePkt->totNumBlocks = 0x00;
	filePkt->currBlockNum = 0x00;
	filePkt->dataBuffLen = 0x00;
	filePkt->padding = 0x00;
	memcpy(filePkt->fileName, fileName, (strlen(fileName) + 1));
	memset(filePkt->dataBuffer, 0, MAX_FILEDATA_SIZE);

	/* Fill Network message strucuture */
	pmsg->cmdType = CMD_REQ;
	pmsg->cmdCode = CMD_FTP;
	pmsg->cmdBufLen = (unsigned char)sizeof(FILEPKT);
	memcpy(pmsg->cmdBuffer, filePkt, sizeof(FILEPKT));

#ifdef ENABLE_FILE_TRANSFER_DEBUG
	printf("FILE_TRANSFER: frameFileRequest()\n");
#endif
#ifdef ENABLE_FILE_TRANSFER_DEBUG
	printf("FILE_TRANSFER: Requst file name          => %s\n", filePkt->fileName);
	printf("FILE_TRANSFER: File Packet size          => 0x%.2X\n", (int)sizeof(FILEPKT));
#endif
	return RET_SUCC;
}

RET_CODE frameFileResponse(PMSG *pmsg, void *cmdStruct, char (*argv)[MAX_PARAM_BUFFER_SIZE], void *cmdArg)
{
	FILEPKT *filePkt = (FILEPKT *)cmdStruct;					/* Extract the File Packet */
	FILEARG *fileArgs = (FILEARG *)cmdArg;						/* Extract Command Arguments */
	/* File related parameters */
	FILE **inFilePtr = &fileArgs->inFilePtr;					/* Reference to input file */
	unsigned long int fileSize = 0;								/* File Size */

#ifdef ENABLE_FILE_TRANSFER_DEBUG
	printf("FILE_TRANSFER: frameFileResponse(), fp = %p\n", *inFilePtr);
#endif

	/* Check for Virgin response to request */	
	if(pmsg->cmdType == CMD_REQ)
	{
		/* Check if File Exists */
		if(FileExists(filePkt->fileName) == FALSE)
		{
			PrintUserMessage("FILE_TRANSFER: frameFileResponse() failed", "file not found");
			/* Issue Response with PARAM_ERR */
			if(frameErrorResponse(pmsg, CMD_PARAM_ERR) != RET_SUCC)
			{
				PrintUserMessage("FILE_TRANSFER: frameEchoRequest() failed", "cannot frame error response");
				return RET_FAIL;
			}
			return RET_SUCC;
		}
		else
		{

#ifdef ENABLE_FILE_TRANSFER_DEBUG
			printf("FILE_TRANSFER: frameFileResponse(), opening file %s in rb mode\n", filePkt->fileName);
#endif
			*inFilePtr = fopen(filePkt->fileName, "rb");
			if(*inFilePtr == NULL)
			{
				PrintUserMessage("FILE_TRANSFER: frameFileResponse() failed", "cannot open file");
				return RET_NULL;
			}
			/* Compute File size in bytes */
			fileSize = GetFileSize(*inFilePtr);
			/* Get Number of File Blocks */
			filePkt->totNumBlocks = CalculateFileBlocks(fileSize, MAX_FILEDATA_SIZE);
			/* Update current block number */
			filePkt->currBlockNum = 1;
			/* Read FILE Block in to data buffer */
			filePkt->dataBuffLen = ReadFileBlock(*inFilePtr, filePkt->dataBuffer, MAX_FILEDATA_SIZE);
			/* Update message type */
			pmsg->cmdType = CMD_RSP;
		}

	}
	/* File transfer in progress */
	else if(pmsg->cmdType == CMD_RSP)
	{
		/* Increment Block Number */
		filePkt->currBlockNum++;
		/* Read FILE Block in to data buffer */
		filePkt->dataBuffLen = ReadFileBlock(*inFilePtr, filePkt->dataBuffer, MAX_FILEDATA_SIZE);
		/* Check for last file block */
		if(filePkt->currBlockNum == filePkt->totNumBlocks)
		{
#ifdef ENABLE_FILE_TRANSFER_DEBUG
			printf("FILE_TRANSFER: Last Block Size          => 0x%.2X\n", filePkt->dataBuffLen);
#endif
			pmsg->cmdType = CMD_RSP_END;
			/* Close the File */
			fclose(*inFilePtr);
			*inFilePtr = NULL;
		}
		else
		{
			pmsg->cmdType = CMD_RSP;
		}

	}
	/* Error in message type, corrupt packet? */
	else
	{
		PrintUserMessage("FILE_TRANSFER: frameFileResponse() failed", "invalid msg type");
		/* Close the File */
		fclose(*inFilePtr);
		*inFilePtr = NULL;

		return RET_FAIL;
	}
#ifdef ENABLE_FILE_TRANSFER_DEBUG
	printf("FILE_TRANSFER: Size of file packet      => 0x%.2X\n", (unsigned char)sizeof(FILEPKT));
	printf("FILE_TRANSFER: File Block size          => 0x%.2X\n", filePkt->dataBuffLen);
#endif
	/* Frame Network Message Packet */
	pmsg->cmdBufLen = (unsigned char)sizeof(FILEPKT);
	memcpy(pmsg->cmdBuffer, filePkt, sizeof(FILEPKT));
	return RET_SUCC;
}

RET_CODE parseFileResponse(const PMSG *pmsg, void *cmdStruct, void *cmdArg)
{
	FILEPKT *filePkt = (FILEPKT *)cmdStruct;					/* Extract the File Packet */
	FILEARG *fileArgs = (FILEARG *)cmdArg;						/* Extract File argument structure */
	/* File related parameters */
	FILE **outFilePtr = &fileArgs->outFilePtr;					/* Reference to output file */
#ifdef ENABLE_PROGRESSBAR
	/* Progress Bar for File Transfer */
	PROGRESS_STRUCT *progressStruct = &fileArgs->progressStruct;
#endif
	
	memcpy(filePkt, pmsg->cmdBuffer, pmsg->cmdBufLen);

#ifdef ENABLE_FILE_TRANSFER_DEBUG
	printf("FILE_TRANSFER: File Packet size          => 0x%.2X\n", (int)filePkt->dataBuffLen);
	printf("FILE_TRANSFER: File Name                 => %s\n", filePkt->fileName);
	printf("FILE_TRANSFER: File Total Blocks         => 0x%.2X\n", filePkt->totNumBlocks);
	printf("FILE_TRANSFER: File Current Block        => 0x%.2X\n", filePkt->currBlockNum);
	printf("FILE_TRANSFER: Command Type              => 0x%.2X\n", pmsg->cmdType);
#endif

#ifdef ENABLE_FILE_TRANSFER_DEBUG
	printf("FILE_TRANSFER: parseFileResponse()\n");
#endif

	/* Check Message Type */	
	if(pmsg->cmdType == CMD_RSP)
	{
		/* Check for Virgin file block */
		if(filePkt->currBlockNum == 1)
		{
			/* Check if File Exists */
			if(FileExists(filePkt->fileName) == TRUE)
			{
				PrintUserMessage("FILE_TRANSFER: parseFileResponse() warning", "overwriting existing file");
			}
#ifdef ENABLE_FILE_TRANSFER_DEBUG
			printf("FILE_TRANSFER: parseFileResponse(), opening file %s in wb mode\n", filePkt->fileName);
#endif

			*outFilePtr = fopen(filePkt->fileName, "wb");
			if(*outFilePtr == NULL)
			{
				PrintUserMessage("FILE_TRANSFER: parseFileResponse() failed", "cannot open file");
				return RET_NULL;
			}
		}

		if(WriteFileBlock(*outFilePtr, filePkt->dataBuffer, filePkt->dataBuffLen) != filePkt->dataBuffLen)
		{
			PrintSystemMessage("FILE_TRANSFER: WriteFileBlock() failed");
		}
#ifdef ENABLE_PROGRESSBAR
		/* Update Progress bar structure */
		progressStruct->currentVal = filePkt->currBlockNum;
		progressStruct->finalVal = filePkt->totNumBlocks;
		if(UpdateProgressBar(progressStruct) == TRUE)
		{
			printf("%s %d%%\r", progressStruct->progressString, progressStruct->percent);
			fflush(stdout);
		}
#endif
	}
	/* Last Response of File Transfer */
	else if(pmsg->cmdType == CMD_RSP_END)
	{
#ifdef ENABLE_FILE_TRANSFER_DEBUG
		printf("FILE_TRANSFER: Writing Last Block of size 0x%.2X\n", filePkt->dataBuffLen);
#endif
		if(WriteFileBlock(*outFilePtr, filePkt->dataBuffer, filePkt->dataBuffLen) != filePkt->dataBuffLen)
		{
			PrintSystemMessage("FILE_TRANSFER: WriteFileBlock() failed");
		}

#ifdef ENABLE_PROGRESSBAR
		/* Update Progress bar structure */
		progressStruct->currentVal = filePkt->currBlockNum;
		progressStruct->finalVal = filePkt->totNumBlocks;
		if(UpdateProgressBar(progressStruct) == TRUE)
		{
			printf("%s %d%%\n\r", progressStruct->progressString, progressStruct->percent);
			fflush(stdout);
		}
#endif
		/* Close the File */
		fclose(*outFilePtr);
		*outFilePtr = NULL;
	}
	/* Error in message type, corrupt packet? */
	else
	{
		PrintUserMessage("FILE_TRANSFER: frameFileResponse() failed", "invalid msg type");
		/* Close the File */
		fclose(*outFilePtr);
		*outFilePtr = NULL;

		return RET_FAIL;
	}

	return RET_SUCC;
}
