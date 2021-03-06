/***********************************************************
 * INCLUDES
 ***********************************************************/
#include "median.h"
#include "bb_predict.h"
#include "math.h"

/***********************************************************
 * FUNCTION
 ***********************************************************/
/**
 * Returns width of Boundingbox.
 * @param bb Boundingbox
 */
float getBbWidth(float* bb)
{
    return abs(bb[2] - bb[0] + 1);
}

/**
 * Returns hight of Boundingbox.
 * @param bb Boundingbox
 */
float getBbHeight(float* bb)
{
    return abs(bb[3] - bb[1] + 1);
}

/**
 * Calculates the new (moved and resized) Bounding box.
 * Calculation based on all relative distance changes of all points
 * to every point. Then the Median of the relative Values is used.
 */
int predictbb(float *bb0, CvPoint2D32f* pt0, CvPoint2D32f* pt1, int nPts,
              float *bb1, float* shift)
{
    float* ofx = (float*) malloc(sizeof(float) * nPts);
    float* ofy = (float*) malloc(sizeof(float) * nPts);
    int i;
    int j;
    int d = 0;
    float dx, dy;
    int lenPdist;
    float* dist0;
    float* dist1;
    float s0, s1;

    for (i = 0; i < nPts; i++)
    {
        ofx[i] = pt1[i].x - pt0[i].x;
        ofy[i] = pt1[i].y - pt0[i].y;
    }

    dx = getMedianUnmanaged(ofx, nPts);
    dy = getMedianUnmanaged(ofy, nPts);

    free(ofx);
    ofx = NULL;
    free(ofy);
    ofy = NULL;

    //m(m-1)/2
    lenPdist = nPts * (nPts - 1) / 2;
    dist0 = (float*) malloc(sizeof(float) * lenPdist);
    dist1 = (float*) malloc(sizeof(float) * lenPdist);

    for (i = 0; i < nPts; i++)
    {
        for (j = i + 1; j < nPts; j++, d++)
        {
            dist0[d] = sqrt(pow(pt0[i].x - pt0[j].x, 2) + pow(pt0[i].y - pt0[j].y, 2));
            dist1[d] = sqrt(pow(pt1[i].x - pt1[j].x, 2) + pow(pt1[i].y - pt1[j].y, 2));
            dist0[d] = dist1[d] / dist0[d];
        }
    }

    //The scale change is the median of all changes of distance.
    //same as s = median(d2./d1) with above
    *shift = getMedianUnmanaged(dist0, lenPdist);

    free(dist0);
    dist0 = NULL;
    free(dist1);
    dist1 = NULL;

    s0 = 0.5 * (*shift - 1) * getBbWidth(bb0);
    s1 = 0.5 * (*shift - 1) * getBbHeight(bb0);

    //Apply transformations (translation& scale) to old Bounding Box
    bb1[0] = bb0[0] - s0 + dx;
    bb1[1] = bb0[1] - s1 + dy;
    bb1[2] = bb0[2] + s0 + dx;
    bb1[3] = bb0[3] + s1 + dy;

    //return absolute scale change
    //shift[0] = s0;
    //shift[1] = s1;

    return 1;
}
