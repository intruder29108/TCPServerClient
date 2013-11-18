/*
 *	MsgProtocol.h
 * 
 *	Author		:	Antony Clince Alex
 *	Date		:	08/11/2013
 *	Description	:	Message strucuture, type and command codes are
 *					defined here
 */

 #ifndef	MSGPROTOCOL_H
 #define	MSGPROTOCOL_H

 /* Macro definitions goes here */

 /* Buffer Constraints */
 #define MAX_CMDBUF_SIZE	252
 #define MAX_MSG_SIZE		266
 /* Message types */
 #define CMD_REQ			0x01
 #define CMD_RSP			0x02
 #define CMD_RSP_ACK		0x03
 #define CMD_RSP_END		0x04
 #define CMD_NOTFOUND		0x05
 #define CMD_PARAM_ERR		0x06
 /* Command Codes */
 #define CMD_ECHO			0x01
 #define CMD_FTP			0x02
 /* Magic Header */
 #define MAGIC_HEADER		"MAGICHEADER"
 #define HEADER_SIZE		11

 typedef struct _PMSG
 {
 	unsigned char cmdType;
 	unsigned char cmdCode;
 	unsigned char cmdBufLen;
 	unsigned char cmdBuffer[MAX_CMDBUF_SIZE];
 } PMSG;

 #endif