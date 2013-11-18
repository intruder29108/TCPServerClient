/*
 *	FileUtility.h
 *
 *	Description	:	Contains Utilty functions for file transfers
 *					like finding file size etc
 *
 *	Author		:	Antony Clince Alex
 *	Date		: 	14/11/2013
 */

#ifndef FILEUTILITY_H
#define	FILEUTILITY_H

/* Project includes */
#include "common.h"

/* Function Declarations */
BOOL 				FileExists(char *);
unsigned long int 	GetFileSize(FILE *);
unsigned char 		ReadFileBlock(FILE *, unsigned char *, unsigned char);
unsigned char 	 	WriteFileBlock(FILE *, unsigned char *, unsigned char);
unsigned int		CalculateFileBlocks(unsigned long int , unsigned char);

#endif
