
/*
 *	FileUtility.c
 *
 *	Description:
 *				Contains functitons to check various
 * 				File attributes like size, R/W access etc
 *
 *	Author	:	Antony Clince Alex
 *	Date	: 	14/11/2013
 */


 /* Standard includes goes here */
 #include <unistd.h>
 #include <stdio.h>

 /* Project includes goes here */
 #include "FileUtility.h"
 
 BOOL FileExists(char *fileName)
 {
 	if(access(fileName, F_OK) == -1)
 	{
 		return FALSE;
 	}
 	return TRUE;
 } 

 unsigned long int GetFileSize(FILE *fp)
 {
 	unsigned long int fileSize = 0;

 	/* Move to end of file */
 	fseek(fp, 0L, SEEK_END);
 	/* Get File Size */
 	fileSize = ftell(fp);
 	/* Reset file pointer */
 	fseek(fp, 0L, SEEK_SET);

 	return fileSize;
 }

 unsigned char ReadFileBlock(FILE *fp, unsigned char *dataBuff, unsigned char blockSize)
 {
 	unsigned char readBytes = 0;

 	if((readBytes = fread(dataBuff, sizeof(unsigned char), blockSize, fp)) != blockSize)
 	{
#ifdef ENABLE_DEBUG
 		printf("ReadFileBlock(), Reading last block, size %d\n", readBytes);
#endif
 	}

 	return readBytes;
 }

 unsigned char WriteFileBlock(FILE *fp, unsigned char *dataBuff, unsigned char blockSize)
 {
 	unsigned char writeBytes = 0;

 	if((writeBytes = fwrite(dataBuff, sizeof(unsigned char), blockSize, fp)) != blockSize)
 	{
#ifdef ENABLE_DEBUG
 		printf("WriteFileBlock(), wrote only %d bytes of %d\n", writeBytes, blockSize);
#endif
 	}

 	return writeBytes;
 }

 unsigned int CalculateFileBlocks(unsigned long int fileSize, unsigned char blockSize)
 {
 	unsigned int numBlocks = 0;

 	if((fileSize % blockSize) == 0)
 	{
 		numBlocks = fileSize / blockSize;
 	}
 	else
 	{
 		numBlocks = (fileSize / blockSize) + 1;
 	}

 	return numBlocks;
 }