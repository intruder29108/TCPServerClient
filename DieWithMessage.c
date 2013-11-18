
/*
 *	DieWithMessage.c
 *
 *	Description:
 *				Simple Utility Function to print Exit Messages
 * 				
 *
 *	Author	:	Antony Clince Alex
 *	Date	: 	07/11/2013
 */

 /* Project Includes Goes here */
 #include <stdio.h>
 #include <stdlib.h>


 void PrintUserMessage(const char *msg, const char *detail)
 {
 	fputs(msg, stderr);
 	fputs(" : ", stderr);
 	fputs(detail, stderr);
 	fputc('\n', stderr);
 	return;
 }

 void PrintSystemMessage(const char *msg)
 {
 	perror(msg);
 }
 
 void DieWithUserMessage(const char *msg, const char *detail)
 {
 	fputs(msg, stderr);
 	fputs(" : ", stderr);
 	fputs(detail, stderr);
 	fputc('\n', stderr);
 	exit(1);
 }

 void DieWithSystemMessage(const char *msg)
 {
 	perror(msg);
 	exit(1);
 }