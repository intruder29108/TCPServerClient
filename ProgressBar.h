/*
 *	ProgressBar.h
 *
 *	Description	:	Implements progress activity indicator
 *					
 *
 *	Author		:	Antony Clince Alex
 *	Date		: 	15/11/2013
 */

#ifndef PROGRESSBAR_H
#define	PROGRESSBAR_H

/* Project includes goes here */
#include "common.h"

/* Macros goes here */

 /* Division factor depeding on lenght of progress string */
 #define DIV_FACTOR	4
 /* Progress bar start character */
 #define PROGRESSBAR_START '['
 /* Progress bar stop character */
 #define PROGRESSBAR_STOP	']'
 /* End of string */
 #define EOS	'\0'
 /* Space */
 #define SPACE 	' '
 /* Progress character */
 #define PROGRESSBAR_CHAR '='
 /* Progress end character */
 #define PROGRESSBAR_END_CHAR '>'
 /* Progress String Lenght */
 #define MAX_PROGRESS_STRING_LEN	28

 /* Typedefs goes here */
 typedef struct _PROGRESS_STRUCT
 {
 	char progressString[MAX_PROGRESS_STRING_LEN];
 	int  currentVal;
 	int  finalVal;
 	int  percent;
 	int  prevScaledPercentage;
 	BOOL isInitialised;
 } PROGRESS_STRUCT;

 void InitialiseProgressBar(PROGRESS_STRUCT *);
 BOOL UpdateProgressBar(PROGRESS_STRUCT *);

#endif
