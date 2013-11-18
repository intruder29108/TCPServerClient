/*
 *	common.h
 *
 *	Description	:	Contains forward references for all utility functions
 *					
 *
 *	Author		:	Antony Clince Alex
 *	Date		: 	07/11/2013
 */

#ifndef COMMON_H
#define	COMMON_H

/* Standard Includes */
#include <stdio.h>
#include <sys/socket.h>

 /* Project Includes */
 

/* Macros and other stuff goes here */


/* Check windows */
#if _WIN32 || _WIN64
#if _WIN64
#define ENVIRONMENT64
#else
#define ENVIRONMENT32
#endif
#endif

/* Check GCC */
#if __GNUC__
#if __x86_64__ || __ppc64__
#define ENVIRONMENT64
#else
#define ENVIRONMENT32
#endif
#endif

#define SUCCESS		   	0
#define FAILURE		   	-1
#define BUFSIZE			10
#define EOS				'\0'
#define NEW_LINE		'\n'

/* Configurable Options */

/* Enable/Disalbe pThreads */
#define USE_PTHREAD
/* Enable TX/RX SM prints */
#undef ENABLE_SM_DEBUG
/* Enable File Transfers */
#define ENABLE_FILE_TRANSFER
/* Enable/Disable for File Transfer */
#undef ENABLE_FILE_TRANSFER_DEBUG
/* Enable Nework Packet Tx Dump */
#undef ENABLE_TX_DUMP
/* Enable Network Packet Rx Dump */
#undef ENABLE_RX_DUMP
/* Enable Network Packet Size Debug */
#undef ENABLE_PKTSIZE_DEBUG
/* Enable Message encoder/decoder prints */
#undef ENABLE_ENCODER_DEBUG
/* Enable Progressbar */
#define ENABLE_PROGRESSBAR
/* Enable Debug for incoming connection */
#undef DEBUG_INCOMING_CON

/* Typedefs and other stuffs */
typedef enum {TRUE = 1, FALSE = 0} BOOL;

/* Function Pointers for state machine implementation */
typedef int 	(*funcPtr)(void *, void *, void *);
typedef funcPtr (*pFuncPtr)(void *, void *, void *);

/* Function forward referencing */
void 	DieWithUserMessage(const char *, const char *);
void 	DieWithSystemMessage(const char *);
void 	PrintUserMessage(const char *, const char *);
void 	PrintSystemMessage(const char *);

void 	PrintSocketAddress(const struct sockaddr *, FILE *);
int 	SetupTCPClientSocket(const char *, const char *);
int		SetupTCPServerSocket(const char *);
int 	AcceptTCPConnection(int);
void 	HandleTCPClient(int);
void 	TCPClientHandler(void *, void *, void *);

#endif
