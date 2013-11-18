/*
 *	MessageFramer.h
 *
 *	Description	:	Forward Referncing to message framing
 *					Functions
 *
 *	Author		:	Antony Clince Alex
 *	Date		: 	08/11/2013
 */

#ifndef MESSAGEFRAMER_H
#define	MESSAGEFRAMER_H

/* Standard Includes */
#include <stdio.h>

/* Forward Referencing */
int GetNetworkMsg(FILE *, unsigned char *, size_t);
int PutNetworkMsg(FILE *, unsigned char *, size_t);

#endif
