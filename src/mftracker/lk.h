/**
 * @file lk.h
 *
 *  Created on: 02.03.2011
 *      @author bernd
 */

/***********************************************************
 * PROLOGUE
 ***********************************************************/

#ifndef LK_H_
#define LK_H_

/***********************************************************
 * INCLUDES
 ***********************************************************/

#include "opencv/cv.h"
#include "opencv/cxcore.h"
#include "highgui.h"

/***********************************************************
 * CONSTANT AND MACRO DEFINITIONS
 ***********************************************************/

/***********************************************************
 * DATA DEFINITIONS
 ***********************************************************/

/***********************************************************
 * FUNCTIONS
 ***********************************************************/
/**
 * Need before start of trackLK and at the end of the program for cleanup.
 */
void initImgs();
int trackLK(IplImage *imgI, IplImage *imgJ, float ptsI[], int nPtsI,
            float ptsJ[], int nPtsJ, int level, float * fbOut, float*nccOut,
            char*statusOut);

#endif /* LK_H_ */

/***********************************************************
 * END OF FILE
 ***********************************************************/
