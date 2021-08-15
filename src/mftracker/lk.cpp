/**
 * @file lk.c
 *
 *  Created on: 28.02.2011
 *      @author Bernd Lukatschek
 */

/***********************************************************
 * INCLUDES
 ***********************************************************/
#include "lk.h"
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <math.h>
#include <stdio.h>

#define SHOW_POINTS     0

/***********************************************************
 * CONSTANT AND MACRO DEFINITIONS
 ***********************************************************/

const int MAX_COUNT = 500;
const int MAX_IMG = 2;
const double N_A_N = -1.0;
/**
 * Size of the search window of each pyramid level in cvCalcOpticalFlowPyrLK.
 */
int win_size_lk = 10;
CvPoint2D32f* points[3] = { 0, 0, 0 };
static IplImage **PYR = 0;

void ShowPoints(IplImage* img, CvPoint2D32f* point, int num, std::string name);

/***********************************************************
 * FUNCTION
 ***********************************************************/
/**
 * Calculates euclidean distance between the point pairs.
 * @param point1    Array of points. Pairs with point2 at every Position.
 * @param point2    Array of points. Pairs with point1 at every Position.
 * @param match     Output: Contains the result of the distance calculation.
 *                  Must have the length of nPts.
 * @param nPts      Number of pairs.
 */
void euclideanDistance(CvPoint2D32f *point1, CvPoint2D32f *point2, float *match, int nPts)
{
    int i;

    for (i = 0; i < nPts; i++)
    {
        match[i] = sqrt((point1[i].x - point2[i].x) * (point1[i].x - point2[i].x)
                        + (point1[i].y - point2[i].y) * (point1[i].y - point2[i].y));
    }
}

/**
 * Calculates normalized cross correlation for every point.
 * @param imgI      Image 1.
 * @param imgJ      Image 2.
 * @param points0   Array of points of imgI
 * @param points1   Array of points of imgJ
 * @param nPts      Length of array/number of points.
 * @param status    Switch which point pairs should be calculated.
 *                  if status[i] == 1 => match[i] is calculated.
 *                  else match[i] = 0.0
 * @param match     Output: Array will contain ncc values.
 *                  0.0 if not calculated.
 * @param winsize   Size of quadratic area around the point
 *                  which is compared.
 * @param method    Specifies the way how image regions are compared.
 *                  see cvMatchTemplate
 */
void normCrossCorrelation(IplImage *imgI, IplImage *imgJ,
                          CvPoint2D32f *points0, CvPoint2D32f *points1, int nPts, char *status,
                          float *match, int winsize, int method)
{
    IplImage *rec0 = cvCreateImage(cvSize(winsize, winsize), 8, 1);
    IplImage *rec1 = cvCreateImage(cvSize(winsize, winsize), 8, 1);
    IplImage *res = cvCreateImage(cvSize(1, 1), IPL_DEPTH_32F, 1);
    int i;

    for (i = 0; i < nPts; i++)
    {
        if (status[i] == 1)
        {
            cvGetRectSubPix(imgI, rec0, points0[i]);
            cvGetRectSubPix(imgJ, rec1, points1[i]);

            /*
            CV_TM_SQDIFF 平方差匹配法：该方法采用平方差来进行匹配；最好的匹配值为0；匹配越差，匹配值越大。
            　　　　　　CV_TM_CCORR 相关匹配法：该方法采用乘法操作；数值越大表明匹配程度越好。
            　　　　　　CV_TM_CCOEFF 相关系数匹配法：1表示完美的匹配；-1表示最差的匹配。
            　　　　　　CV_TM_SQDIFF_NORMED 归一化平方差匹配法
            　　　　　　CV_TM_CCORR_NORMED 归一化相关匹配法
            　　　　　　CV_TM_CCOEFF_NORMED 归一化相关系数匹配法*/
            cvMatchTemplate(rec0, rec1, res, method);

            match[i] = ((float *)(res->imageData))[0];
        }
        else
        {
            match[i] = 0.0;
        }
    }

    cvReleaseImage(&rec0);
    cvReleaseImage(&rec1);
    cvReleaseImage(&res);
}

/**
 * Needed before start of trackLK and at the end of the program for cleanup.
 * Handles PYR(Pyramid cache) variable.
 */
void initImgs()
{
    if (PYR != 0)
    {
        int i;

        for (i = 0; i < MAX_IMG; i++)
        {
            cvReleaseImage(&(PYR[i]));
            PYR[i] = 0;
        }

        free(PYR);
        PYR = 0;
        //printf("LK: deallocated\n");
    }

    PYR = (IplImage**) calloc(MAX_IMG, sizeof(IplImage*));
    //printf("LK: initialized\n");
}

/**
 * Tracks Points from 1.Image to 2.Image.
 * Need initImgs before start and at the end of the program for cleanup.
 *
 * @param imgI      previous Image source. (isn't changed)
 * @param imgJ      actual Image target. (isn't changed)
 * @param ptsI      points to track from first Image.
 *                  Format [0] = x1, [1] = y1, [2] = x2 ...
 * @param nPtsI     number of Points to track from first Image
 * @param ptsJ      container for calculated points of second Image.
 *                  Must have the length of nPtsI.
 * @param nPtsJ     number of Points
 * @param level     Pyramidlevel, default 5
 * @param fb        forward-backward confidence value.
 *                  (corresponds to euclidean distance between).
 *                  Must have the length of nPtsI: nPtsI * sizeof(float).
 * @param ncc       normCrossCorrelation values. needs as inputlength nPtsI * sizeof(float)
 * @param status    Indicates positive tracks. 1 = PosTrack 0 = NegTrack
 *                  needs as inputlength nPtsI * sizeof(char)
 *
 *
 * Based Matlab function:
 * lk(2,imgI,imgJ,ptsI,ptsJ,Level) (Level is optional)
 */
int trackLK(IplImage *imgI, IplImage *imgJ,
            float ptsI[], int nPtsI,
            float ptsJ[], int nPtsJ,
            int level, float * fb, float*ncc, char*status)
{
    //TODO: watch NaN cases
    //double nan = std::numeric_limits<double>::quiet_NaN();
    //double inf = std::numeric_limits<double>::infinity();

    // tracking
    int I, J, winsize_ncc;
    CvSize pyr_sz;
    int i;

    //if unused std 5
    if (level == -1)
    {
        level = 5;
    }

    I = 0;
    J = 1;
    winsize_ncc = 10;

    //NOTE: initImgs() must be used correctly or memleak will follow.
    pyr_sz = cvSize(imgI->width + 8, imgI->height / 3);
    PYR[I] = cvCreateImage(pyr_sz, IPL_DEPTH_32F, 1);
    PYR[J] = cvCreateImage(pyr_sz, IPL_DEPTH_32F, 1);

    // Points
    if (nPtsJ != nPtsI)
    {
        printf("Inconsistent input!\n");
        return 0;
    }

    points[0] = (CvPoint2D32f*) malloc(nPtsI * sizeof(CvPoint2D32f)); // template
    points[1] = (CvPoint2D32f*) malloc(nPtsI * sizeof(CvPoint2D32f)); // target
    points[2] = (CvPoint2D32f*) malloc(nPtsI * sizeof(CvPoint2D32f)); // forward-backward

    char* statusBacktrack = (char*) malloc(nPtsI);

    for (i = 0; i < nPtsI; i++)
    {
        points[0][i].x = ptsI[2 * i + 0];
        points[0][i].y = ptsI[2 * i + 1];
        points[1][i].x = ptsJ[2 * i + 0];
        points[1][i].y = ptsJ[2 * i + 1];
        points[2][i].x = ptsI[2 * i + 0];
        points[2][i].y = ptsI[2 * i + 1];
    }

    //showPoints(imgI, points[0], nPtsI);

    //lucas kanade track
    cvCalcOpticalFlowPyrLK(imgI, imgJ, PYR[I], PYR[J], points[0], points[1],
                           nPtsI, cvSize(win_size_lk, win_size_lk), level, status, 0,
                           cvTermCriteria(CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 20, 0.03),
                           CV_LKFLOW_INITIAL_GUESSES);

#if SHOW_POINTS
    ShowPoints(imgI, points[0], nPtsI, "frame1");
    ShowPoints(imgJ, points[1], nPtsI, "frame2");
#endif

    //backtrack
    cvCalcOpticalFlowPyrLK(imgJ, imgI, PYR[J], PYR[I], points[1], points[2],
                           nPtsI, cvSize(win_size_lk, win_size_lk), level, statusBacktrack, 0,
                           cvTermCriteria(CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 20, 0.03),
                           CV_LKFLOW_INITIAL_GUESSES | CV_LKFLOW_PYR_A_READY | CV_LKFLOW_PYR_B_READY);
#if SHOW_POINTS
    ShowPoints(imgJ, points[1], nPtsI, "frame11");
#endif

    for (i = 0; i < nPtsI; i++)
    {
        if (status[i] && statusBacktrack[i])
        {
            status[i] = 1;
        }
        else
        {
            status[i] = 0;
        }
    }

#if SHOW_POINTS
    {
        int cnt = 0;
        CvPoint2D32f* dp = (CvPoint2D32f*)malloc(nPtsI * sizeof(CvPoint2D32f));

        for (i = 0; i < nPtsI; i++)
        {
            if (status[i] && statusBacktrack[i])
            {
                dp[cnt++] = points[1][i];
            }
        }

        ShowPoints(imgJ, dp, cnt, "right point");

        free(dp);
    }
#endif

    normCrossCorrelation(imgI, imgJ, points[0], points[1], nPtsI, status, ncc,
                         winsize_ncc, CV_TM_CCOEFF_NORMED);

    euclideanDistance(points[0], points[2], fb, nPtsI);

    for (i = 0; i < nPtsI; i++)
    {
        if (status[i] == 1)
        {
            ptsJ[2 * i + 0] = points[1][i].x;
            ptsJ[2 * i + 1] = points[1][i].y;
        }
        else //flow for the corresponding feature hasn't been found
        {
            //Todo: shell realy write N_A_N in it?
            ptsJ[2 * i + 0] = N_A_N;
            ptsJ[2 * i + 1] = N_A_N;
            fb[i] = N_A_N;
            ncc[i] = N_A_N;
        }
    }

    for (i = 0; i < 3; i++)
    {
        free(points[i]);
        points[i] = NULL;
    }

    free(statusBacktrack);

    return 1;
}

void ShowPoints(IplImage* img, CvPoint2D32f* point, int num, std::string name)
{
    IplImage* dst = cvCreateImage(cvSize(img->width, img->height), img->depth, 3);
    cvConvertImage(img, dst, CV_GRAY2BGR);

    for (int i = 0; i < num; i++)
    {
        cvLine(dst, cvPoint(point[i].x, point[i].y), cvPoint(point[i].x, point[i].y), cvScalar(0, 0, 255, 0) , 2, 8, 0);
    }

    cvNamedWindow(name.c_str());
    cvShowImage(name.c_str(), dst);
    cvWaitKey(0);
    cvReleaseImage(&dst);
    cvDestroyWindow(name.c_str());
}

