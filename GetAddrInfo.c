
/*
 *	GetAddrInfo.c
 *
 *	Description:
 *				Simple Program to illustrate use of getaddrinfo function
 * 				Call.
 *
 *	Author	:	Antony Clince Alex
 *	Date	: 	06/11/13
 */

 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include <netdb.h>

 /* Forward Referencing */
 void PrintSocketAddress(const struct sockaddr *address);

 int main(int argc, char *argv[])
 {
 	if(argc != 3)
 	{
 		printf("Usage : GetAddrInfo <Address/Name> <Port/Service>\n");
 		return -1;
 	}

 	char *addrString = argv[1];
 	char *portString = argv[2];

 	/* Set criteria of address search */
 	struct addrinfo addCriteria;
 	memset(&addCriteria, 0, sizeof(addCriteria));
 	addCriteria.ai_family = AF_INET;
 	addCriteria.ai_socktype = SOCK_STREAM;
 	addCriteria.ai_protocol = IPPROTO_TCP;

 	/* Get Address associated with specified name and port */
 	struct addrinfo *addrList;
 	struct addrinfo *addr;
 	int rtnVal = getaddrinfo(addrString, portString, &addCriteria, &addrList);
 	if(rtnVal != 0)
 	{
 		printf("getaddrinfo() : failed !!!\n");
 		return -1;
 	}


 	for(addr = addrList; addr != NULL; addr = addr->ai_next)
 	{
 		PrintSocketAddress(addr->ai_addr);
 		printf("\n");
 	}

 	/* Free the addrinfo Linked list */
 	freeaddrinfo(addrList);

 	return 0;
 }

 /* Utility function definitions */
 void PrintSocketAddress(const struct sockaddr *address)
 {
 	/* NULL Pointer Check */
 	if(address == NULL)
 		return;

 	void *numericAdress;
 	char addrBuffer[INET6_ADDRSTRLEN];
 	in_port_t port;

 	switch(address->sa_family)
 	{
 		case AF_INET:
 		numericAdress = &((struct sockaddr_in *)address)->sin_addr;
 		port = ntohs(((struct sockaddr_in *)address)->sin_port);
 		break;
 		case AF_INET6:
 		numericAdress = &((struct sockaddr_in6 *)address)->sin6_addr;
 		port = ntohs(((struct sockaddr_in6 *)address)->sin6_port);
 		break;
 		default:
 		printf("Unknown Stream !!!");
 		return;
 	}
 	/* Convert numeric address to printable form */
 	if(inet_ntop(address->sa_family, numericAdress, addrBuffer, sizeof(addrBuffer)) == NULL)
 	{
 		printf("inet_ntop() : Invalid address !!!\n");
 		return;
 	}
 	else
 	{
 		printf("%s", addrBuffer);
 		if(port != 0)
 		{
 			printf("-%u", port);
 		}
 	}
 	printf("\n");
 }