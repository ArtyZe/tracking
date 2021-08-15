/**
 * @file track.c
 *
 *  Created on: 01.03.2011
 *      @author bernd
 */

/***********************************************************
 * INCLUDES
 ***********************************************************/

#include "bb.h"
#include "bb_predict.h"
#include "median.h"
#include "stdio.h"
#include "lk.h"

/***********************************************************
 * FUNCTION
 ***********************************************************/

/**
 * Calculate the bounding box of an Object in a following Image.
 * Imgs aren't changed.
 * @param imgI       Image contain Object with known BoundingBox
 * @param imgJ       Following Image.
 * @param bb         Bounding box of object to track in imgI.
 *                   Format x1,y1,x2,y2
 * @param scaleshift returns relative scale change of bb
 */
int fbtrack(IplImage *imgI, IplImage *imgJ, float* bb, float* bbnew, float* scaleshift)
{
    char level = 5;
    int numM = 10;
    int numN = 10;
    int nPoints = numM * numN;
    int sizePointsArray = nPoints * 2;
    float* fb = (float*) malloc(nPoints * sizeof(float));
    float* ncc = (float*) malloc(nPoints * sizeof(float));
    char* status = (char*) malloc(nPoints);
    float * pt = (float*) malloc(sizeof(float) * sizePointsArray);
    float * ptTracked = (float*) malloc(sizeof(float) * sizePointsArray);
    int nlkPoints;
    CvPoint2D32f* startPoints;
    CvPoint2D32f* targetPoints;
    float *fbLkCleaned;
    float *nccLkCleaned;
    int i, M;
    int nRealPoints;
    float medFb;
    float medNcc;
    int nAfterFbUsage;

#if 1
    getFilledBBPoints(bb, numM, numN, 5, &pt);

    memcpy(ptTracked, pt, sizeof(float) * sizePointsArray);
#else
    IplImage *eig_image = cvCreateImage(cvSize(imgI->width, imgI->height), IPL_DEPTH_32F, 1);
    IplImage *tmp_image = cvCreateImage(cvSize(imgI->width, imgI->height), IPL_DEPTH_32F, 1);
    int k;
    int corners_count = nPoints;
    CvPoint2D32f *cornersA = new CvPoint2D32f[nPoints];
    CvRect rect = {bb[0], bb[1], bb[2] - bb[0] + 1, bb[3] - bb[1] + 1};

    cvSetImageROI(imgI, rect);
    cvSetImageROI(eig_image, rect);
    cvSetImageROI(tmp_image, rect);

    cvGoodFeaturesToTrack(imgI, eig_image, tmp_image, cornersA, &corners_count, 0.01, 2.0, NULL, 3, 0, 0.04);

    /*if (imgI->width > 10 * 2 + 6 && imgI->height > 10 * 2 + 6)
    {
    cvFindCornerSubPix(imgI, cornersA, corners_count, cvSize(10, 10), cvSize(-1, -1),
    cvTermCriteria((CV_TERMCRIT_ITER | CV_TERMCRIT_EPS), 20, 0.03));
    }*/

    for (k = 0; k < corners_count; k++)
    {
        cornersA[k].x += bb[0];
        cornersA[k].y += bb[1];
    }

    cvResetImageROI(imgI);
    cvResetImageROI(eig_image);
    cvResetImageROI(tmp_image);

    corners_count = MIN(corners_count, nPoints);

    if (corners_count > 0)
    {
        memcpy(pt, cornersA, sizeof(CvPoint2D32f) * corners_count);
        memcpy(ptTracked, cornersA, sizeof(CvPoint2D32f) * corners_count);
    }

    nPoints = corners_count;

    cvReleaseImage(&eig_image);
    cvReleaseImage(&tmp_image);

    delete []cornersA;

    printf("Number of points before lk: %d\n", nPoints);
#endif

    initImgs();
    trackLK(imgI, imgJ, pt, nPoints, ptTracked, nPoints, level, fb, ncc, status);
    initImgs();

    nlkPoints = 0;

    for (i = 0; i < nPoints; i++)
    {
        nlkPoints += status[i];
    }

    if (nlkPoints == 0)
    {
        return 0;
    }

    startPoints = (CvPoint2D32f*) malloc(nlkPoints * sizeof(CvPoint2D32f));
    targetPoints = (CvPoint2D32f*) malloc(nlkPoints * sizeof(CvPoint2D32f));
    fbLkCleaned = (float*) malloc(nlkPoints * sizeof(float));
    nccLkCleaned = (float*) malloc(nlkPoints * sizeof(float));

    M = 2;
    nRealPoints = 0;

    for (i = 0; i < nPoints; i++)
    {
        //TODO:handle Missing Points or status[i]==0
        if (ptTracked[M * i] == -1)
        {
        }
        else
        {
            startPoints[nRealPoints].x = pt[2 * i + 0];
            startPoints[nRealPoints].y = pt[2 * i + 1];
            targetPoints[nRealPoints].x = ptTracked[2 * i + 0];
            targetPoints[nRealPoints].y = ptTracked[2 * i + 1];
            fbLkCleaned[nRealPoints] = fb[i];
            nccLkCleaned[nRealPoints] = ncc[i];
            nRealPoints++;
        }
    }

    free(pt);
    free(ptTracked);

    //assert nRealPoints==nlkPoints
    medFb = getMedian(fbLkCleaned, nlkPoints);
    medNcc = getMedian(nccLkCleaned, nlkPoints);
    printf("medianfb: %f --- medianncc: %f --- ", medFb, medNcc);
    printf("Number of points  after LK: %d\n", nlkPoints);

    nAfterFbUsage = 0;

    for (i = 0; i < nlkPoints; i++)
    {
        if ((fbLkCleaned[i] <= medFb) & (nccLkCleaned[i] >= medNcc))
        {
            startPoints[nAfterFbUsage] = startPoints[i];
            targetPoints[nAfterFbUsage] = targetPoints[i];
            nAfterFbUsage++;
        }
    }

#if 0
    printf("Number of points after fb correction: %d\n", nAfterFbUsage);
    showIplImage(imgI);
    // show "OpticalFlow" fb filtered.
    drawLines(imgI, startPoints, nRealPoints, targetPoints, nRealPoints);
    showIplImage(imgI);
#endif

    predictbb(bb, startPoints, targetPoints, nAfterFbUsage, bbnew, scaleshift);

#if 0
    printf("bbnew: %f,%f,%f,%f\n", bbnew[0], bbnew[1], bbnew[2], bbnew[3]);
    printf("relative scale: %f \n", scaleshift[0]);
#endif

    //show picture with tracked bb
    //drawRectFromBB(imgJ, bbnew);
    //showIplImage(imgJ);

    free(fb);
    free(ncc);
    free(status);
    free(startPoints);
    free(targetPoints);
    free(fbLkCleaned);
    free(nccLkCleaned);

    if (medFb > 10)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}
