
/*
 *	ProgressBar.c
 *
 *	Description:
 *				Implements simple progress activity
 * 				indicator
 *
 *	Author	:	Antony Clince Alex
 *	Date	: 	15/11/2013
 */

 /* Standard includes goes here */
 #include <string.h>
 
 /* Project includes goes here */
 #include "ProgressBar.h"
 

void InitialiseProgressBar(PROGRESS_STRUCT *progressStruct)
{
	int iVal = 0;

	/* Initialise the progress string */
	progressStruct->progressString[iVal++] = PROGRESSBAR_START;
	while(iVal < MAX_PROGRESS_STRING_LEN)
	{
		if(iVal == (MAX_PROGRESS_STRING_LEN - 1))
		{
			progressStruct->progressString[iVal] = PROGRESSBAR_STOP;
		}
		else
		{
			progressStruct->progressString[iVal] = SPACE;
		}
		iVal++;
	}
	progressStruct->progressString[iVal] = EOS;

	/* Mark structure as initialised */
	progressStruct->isInitialised = TRUE;
	progressStruct->percent = 0;
	progressStruct->prevScaledPercentage = 0;

	return;
}

 BOOL UpdateProgressBar(PROGRESS_STRUCT *progressStruct)
 {
 	unsigned char percent = 0, iVal = 0;


 	/* Check if progress string is initialised */
 	if(progressStruct->isInitialised != TRUE)
 	{
 		/* Initialise it */
 		InitialiseProgressBar(progressStruct);
 	}

 	/* Calculate percentage completion */
 	progressStruct->percent = ((progressStruct->currentVal * 100) / progressStruct->finalVal);
 	/* Scale percentage to 0 to 25 */
 	percent = progressStruct->percent / DIV_FACTOR;
 	/* Update progress string only if change in percentage */
 	if(progressStruct->prevScaledPercentage != percent)
 	{
	 	/* Update Progress String */
	 	for(iVal = 1; iVal <= percent; iVal++)
	 	{
	 		progressStruct->progressString[iVal] = PROGRESSBAR_CHAR;
	 	}
	 	progressStruct->progressString[iVal] = PROGRESSBAR_END_CHAR;
	 	progressStruct->progressString[MAX_PROGRESS_STRING_LEN] = EOS;
	 	progressStruct->prevScaledPercentage = percent;
	 	/* Indicate to end user the change in progress */
	 	return TRUE;
	 }
	 	
	/* No change in progress, there is no need to reprint */
 	return FALSE;
 }