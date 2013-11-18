/*
 *	MessageEncoder.h
 *
 *	Description	:	Contains Forward referencing for
 *					encoding and decoding network messages
 *
 *	Author		:	Antony Clince Alex
 *	Date		: 	08/11/2013
 */

#ifndef MESSAGEENCODER_H
#define	MESSAGEENCODER_H

 /* Standard Includes */
 
 /* Project Includes */
 #include "MsgProtocol.h"

/* Forward Referencing */
size_t EncodeNetworkMsg(const PMSG *, unsigned char *, size_t);
int DecodeNetworkMsg(PMSG *, unsigned char *, size_t);


#endif
