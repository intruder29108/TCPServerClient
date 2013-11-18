/*
 *	commandTable.h
 *
 *	Description	:	Structure definitions for individual commands,
 *					function pointers and others stuff
 *
 *	Author		:	Antony Clince Alex
 *	Date		: 	08/11/2013
 */

#ifndef COMMANDTABLE_H
#define	COMMANDTABLE_H

/* Standard Includes */

/* Project Includes */
#include "common.h"
#include "MsgProtocol.h"
#include "ProgressBar.h"

/* Macros Definitions goes here */
#define MAX_FILENNAME_SIZE			18
#define MAX_FILEDATA_SIZE			224
#define MAX_NUM_CMDS				50
#define MAX_PARAM_BUFFER_SIZE 		30
#define MAX_NUM_PARAMS				10
#define MAX_CMDARG_STRUCT_SIZE		256

/* Size definitions for various structures */
#define INT_SIZE 					4
#ifdef ENVIRONMENT64
#define FILE_PTR_SIZE				8
#else
#define FILE_PTR_SIZE				4
#endif
#define PROGRESS_STRUCT_SIZE		48

/* Typdefs and Enumerations goes here */
typedef enum {RET_SUCC, RET_FAIL, RET_NULL, RET_ERR1, RET_ERR2} RET_CODE;
typedef struct _CMDTABLE
{
	unsigned char cmdName[MAX_PARAM_BUFFER_SIZE];
	unsigned char cmdCode;
	unsigned char numArgs;
	RET_CODE (*parseRequestPacket)(const PMSG *, void *, void *);
	RET_CODE (*frameRequestPacket)(PMSG *, void *, char (*)[MAX_PARAM_BUFFER_SIZE], void *);
	RET_CODE (*frameResponsePacket)(PMSG *, void *, char (*)[MAX_PARAM_BUFFER_SIZE], void *);
	RET_CODE (*parseResponsePacket)(const PMSG *, void *, void *);
} CMDTABLE;

/* Datastructures for various commands */

/* Generic Command Structure */
typedef struct _CMDSTRUCT
{
	unsigned char data[MAX_CMDBUF_SIZE];
} CMDSTRUCT;
/* Echo Data Format */
typedef struct _ECHOPKT
{
	unsigned char payload[MAX_CMDBUF_SIZE];
} ECHOPKT;
/* File Transfer data format */
typedef struct _FILEPKT
{
	unsigned int  totNumBlocks;						/* Total number of file blocks */
	unsigned int  currBlockNum;						/* Current Block Number */
	unsigned char dataBuffLen;						/* Data buffer length */
	unsigned char padding;							/* Padding for boundary */
	unsigned char fileName[MAX_FILENNAME_SIZE];		/* File Name */
	unsigned char dataBuffer[MAX_FILEDATA_SIZE];	/* Contains file data buffer */
} FILEPKT;
/* Client SM Argument Structure */
typedef struct _CLNTARG
{
	int cmdIdx;
	char (*argv)[MAX_PARAM_BUFFER_SIZE];
	FILE *filePtr;
} CLNTARG;
/* Generic Command Argument Strucuture */
typedef struct _CMDARG
{
	int cmdIdx;
	PROGRESS_STRUCT progressStruct;
	unsigned char padding[MAX_CMDARG_STRUCT_SIZE - (PROGRESS_STRUCT_SIZE + INT_SIZE)];						/* Align to 256 bytes*/
} CMDARG;
/* Command Argument Structure for Echo Command */
typedef struct _ECHOARG
{
	int cmdIdx;
	PROGRESS_STRUCT progressStruct;
	unsigned char padding[MAX_CMDARG_STRUCT_SIZE - (PROGRESS_STRUCT_SIZE + INT_SIZE)];						/* Align to 256 bytes */
} ECHOARG;
/* Command Argument Structure for File Command */
typedef struct _FILEARG
{
	int cmdIdx;
	FILE *inFilePtr;
	FILE *outFilePtr;
	PROGRESS_STRUCT progressStruct;
	unsigned char padding[MAX_CMDARG_STRUCT_SIZE - (PROGRESS_STRUCT_SIZE + INT_SIZE + 2*FILE_PTR_SIZE)];	/* Align to 256 bytes */
} FILEARG;
/* Function Forward Declarations */

/* Echo Routine */
RET_CODE parseEchoRequest(const PMSG *, void *, void *);
RET_CODE frameEchoRequest(PMSG *, void *, char (*)[MAX_PARAM_BUFFER_SIZE], void *);
RET_CODE frameEchoResponse(PMSG *, void *, char (*)[MAX_PARAM_BUFFER_SIZE], void *);
RET_CODE parseEchoResponse(const PMSG *, void *, void *);

/* File Transfer Routine */
RET_CODE parseFileRequest(const PMSG *, void *, void *);
RET_CODE frameFileRequest(PMSG *, void *, char (*)[MAX_PARAM_BUFFER_SIZE], void *);
RET_CODE frameFileResponse(PMSG *, void *, char (*)[MAX_PARAM_BUFFER_SIZE], void *);
RET_CODE parseFileResponse(const PMSG *, void *, void *);

/* Common routine */
RET_CODE frameErrorResponse(PMSG *, int);


#endif
