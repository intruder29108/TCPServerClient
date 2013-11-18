
/*
 *	AddressUtility.c
 *
 *	Description:
 *				Implemention to extract and print address and port from 
 * 				Generic socket address strucutre
 *
 *	Author	:	Antony Clince Alex
 *	Date	: 	07/11/2013
 */

 /* Standard includes goes here */
 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include <unistd.h>
 #include <netdb.h>

/* Project includes goes here */
#include "common.h"

 void PrintSocketAddress(const struct sockaddr *address, FILE *stream)
 {
 	/* NULL Pointer Check */
 	if(address == NULL || stream == NULL)
 		return;

 	void *numericAddress;				/* Pointer to binary address */
 	char addrBuffer[INET6_ADDRSTRLEN];	/* Char array large enough to hold v6 address */
 	in_port_t port;						/* Port number to print */

 	/* Socket Family check for v4 or v6 */
 	switch(address->sa_family)
 	{
 		case AF_INET:
 		numericAddress = &((struct sockaddr_in *)address)->sin_addr;
		port = ntohs(((struct sockaddr_in *)address)->sin_port);
		break;
		case AF_INET6:
		numericAddress = &((struct sockaddr_in6 *)address)->sin6_addr;
		port = ntohs(((struct sockaddr_in6 *)address)->sin6_port);
		break;
		default:
		fputs("[unknown type]", stream);
		return;
 	}

 	/* Convert address in binary to printable form */
 	if(inet_ntop(address->sa_family, numericAddress, addrBuffer, sizeof(addrBuffer)) == NULL)
 	{
 		fputs("[invalid address]", stream);			/* Unable to convert */
 	}
 	else
 	{
 		fprintf(stream, "%s", addrBuffer);
 		if(port != 0)
 			fprintf(stream, "-%u", port);
 	}
 }