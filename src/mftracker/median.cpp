#include <stdlib.h>
#include <string.h>
/**
 * @file median.c
 *
 *  Created on: 22.03.2011
 *     @author: bernd
 * @brief
 */

#define ELEM_SWAP(a,b) { register float t=(a);(a)=(b);(b)=t; }

/***********************************************************
 * INCLUDES
 ***********************************************************/
#include "median.h"

/***********************************************************
 * FUNCTION
 ***********************************************************/
/**
 * Returns median of the array. Changes array!
 * @param arr the array
 * @pram n length of array
 *
 *  This Quickselect routine is based on the algorithm described in
 *  "Numerical recipes in C", Second Edition,
 *  Cambridge University Press, 1992, Section 8.5, ISBN 0-521-43108-5
 *  This code by Nicolas Devillard - 1998. Public domain.
 */
float getMedianUnmanaged(float arr[], int n)
{
    int low, high;
    int median;
    int middle, ll, hh;

    low = 0;
    high = n - 1;
    median = (low + high) / 2;

    for (;;)
    {
        if (high <= low) /* One element only */
        {
            return arr[median];
        }

        if (high == low + 1)
        {
            /* Two elements only */
            if (arr[low] > arr[high])
            {
                ELEM_SWAP(arr[low], arr[high]);
            }

            return arr[median];
        }

        /* Find median of low, middle and high items; swap into position low */
        middle = (low + high) / 2;

        if (arr[middle] > arr[high])
        {
            ELEM_SWAP(arr[middle], arr[high]);
        }

        if (arr[low] > arr[high])
        {
            ELEM_SWAP(arr[low], arr[high]);
        }

        if (arr[middle] > arr[low])
        {
            ELEM_SWAP(arr[middle], arr[low]);
        }

        /* Swap low item (now in position middle) into position (low+1) */
        ELEM_SWAP(arr[middle], arr[low + 1]);

        /* Nibble from each end towards middle, swapping items when stuck */
        ll = low + 1;
        hh = high;

        for (;;)
        {
            do
            {
                ll++;
            }
            while (arr[low] > arr[ll]);

            do
            {
                hh--;
            }
            while (arr[hh] > arr[low]);

            if (hh < ll)
            {
                break;
            }

            ELEM_SWAP(arr[ll], arr[hh]);
        }

        /* Swap middle item (in position low) back into correct position */
        ELEM_SWAP(arr[low], arr[hh]);

        /* Re-set active partition */
        if (hh <= median)
        {
            low = ll;
        }

        if (hh >= median)
        {
            high = hh - 1;
        }
    }
}

/**
 * Calculates Median of the array. Don't change array(makes copy).
 * @param arr the array
 * @pram n length of array
 */
float getMedian(float arr[], int n)
{
    float *temP = (float*) malloc(sizeof(float) * n);
    //  int i;
    //  for (i = 0; i < n; i++)
    //  {
    //    temP[i] = arr[i];
    //  }
    memcpy(temP, arr, sizeof(float) * n);

    float median;
    median = getMedianUnmanaged(temP, n);

    free(temP);

    return median;
}

#undef ELEM_SWAP
