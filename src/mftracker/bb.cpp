/**
 * @file bb.c
 *
 *  Created on: 17.03.2011
 *     @author: bernd
 * @brief
 */

/***********************************************************
 * INCLUDES
 ***********************************************************/
#include "bb.h"

int calculateBBCenter(float** bb, float**center);

/***********************************************************
 * FUNCTION
 ***********************************************************/
/**
 * Creates numM x numN points grid on BBox.
 * Points ordered in 1 dimensional array (x1, y1, x2, y2).
 * @param bb        Bounding box represented through 2 points(x1,y1,x2,y2)
 * @param numM      Number of points in height direction.
 * @param numN      Number of points in width direction.
 * @param margin    margin (in pixel)
 * @param pts       Contains the calculated points in the form (x1, y1, x2, y2).
 *                  Size of the array must be numM * numN * 2.
 */
int getFilledBBPoints(float *bb, int numM, int numN, int margin, float**pts)
{
    float* pt = *pts;
    int pointDim = 2;
    int i;
    int j;
    float* pbb_local;
    /**
     * gap between points in width direction
     */
    float divN;
    /**
     * gap between points in height direction
     */
    float divM;
    float bb_local[4];
    float center[2];
    float spaceN;
    float spaceM;
    float *cen;

    /*add margin*/
    bb_local[0] = bb[0] + margin;
    bb_local[1] = bb[1] + margin;
    bb_local[2] = bb[2] - margin;
    bb_local[3] = bb[3] - margin;
    pbb_local = bb_local;

    /*  printf("PointArraySize should be: %d\n", numM * numN * pointDim);*/
    /*handle cases numX = 1*/
    if (numN == 1 && numM == 1)
    {
        calculateBBCenter(&pbb_local, pts);
        return 1;
    }
    else if (numN == 1 && numM > 1)
    {
        divM = numM - 1;
        divN = 2;
        /*maybe save center coordinate into bb[1] instead of loop again*/
        /*calculate step width*/
        spaceM = (bb_local[3] - bb_local[1]) / divM;
        cen = center;
        calculateBBCenter(&pbb_local, &cen);

        /*calculate points and save them to the array*/
        for (i = 0; i < numN; i++)
        {
            for (j = 0; j < numM; j++)
            {
                pt[i * numM * pointDim + j * pointDim + 0] = cen[0];
                pt[i * numM * pointDim + j * pointDim + 1] = bb_local[1] + j * spaceM;
            }
        }

        return 1;
    }
    else if (numN > 1 && numM == 1)
    {
        float center[2];
        float *cen;

        divM = 2;
        divN = numN - 1;

        //maybe save center coordinate into bb[1] instead of loop again
        //calculate step width
        spaceN = (bb_local[2] - bb_local[0]) / divN;
        cen = center;
        calculateBBCenter(&pbb_local, &cen);

        //calculate points and save them to the array
        for (i = 0; i < numN; i++)
        {
            for (j = 0; j < numM; j++)
            {
                pt[i * numM * pointDim + j * pointDim + 0] = bb_local[0] + i * spaceN;
                pt[i * numM * pointDim + j * pointDim + 1] = cen[1];
            }
        }

        return 1;
    }
    else if (numN > 1 && numM > 1)
    {
        divM = numM - 1;
        divN = numN - 1;
    }

    //calculate step width
    spaceN = (bb_local[2] - bb_local[0]) / divN;
    spaceM = (bb_local[3] - bb_local[1]) / divM;

    //calculate points and save them to the array
    for (i = 0; i < numN; i++)
    {
        for (j = 0; j < numM; j++)
        {
            pt[i * numM * pointDim + j * pointDim + 0] = bb_local[0] + i * spaceN;
            pt[i * numM * pointDim + j * pointDim + 1] = bb_local[1] + j * spaceM;
        }
    }

    return 1;
}

/**
 * Calculates center of a Rectangle/Boundingbox.
 * @param bb defined with 2 points x,y,x1,y1
 * @param center point center[0]=x,center[1]=y
 */
int calculateBBCenter(float** bb, float**center)
{
    float* bbnow = *bb;
    float* centernow = *center;

    if (bbnow == 0)
    {
        centernow = 0;
        return 0;
    }

    centernow[0] = 0.5 * (bbnow[0] + bbnow[2]);
    centernow[1] = 0.5 * (bbnow[1] + bbnow[3]);

    return 1;
}
