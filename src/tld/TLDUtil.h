/*
 * util.h
 *
 *  Created on: 30.05.2011
 *      Author: Georg Nebehay
 */

#ifndef TLDUTIL_H_
#define TLDUTIL_H_

#include <utility>
#include <opencv/cv.h>

using namespace cv;
using namespace std;

namespace tld
{

template <class T1, class T2>
void tldConvertBB(T1 * src, T2 * dest)
{
    dest[0] = src[0];
    dest[1] = src[1];
    dest[2] = src[2];
    dest[3] = src[3];
}

template <class T>
void tldCopyBB(T * src, T * dest)
{
    tldConvertBB<T, T>(src, dest);
}

template <class T>
void tldCopyBoundaryToArray(T x, T y, T width, T height, T * array)
{
    array[0] = x;
    array[1] = y;
    array[2] = width;
    array[3] = height;
}

template <class T>
void tldExtractDimsFromArray(T * boundary, T * x, T * y, T *width, T * height)
{
    *x = boundary[0];
    *y = boundary[1];
    *width = boundary[2];
    *height = boundary[3];
}

template <class T>
void tldRectToArray(Rect rect, T * boundary)
{
    boundary[0] = rect.x;
    boundary[1] = rect.y;
    boundary[2] = rect.width;
    boundary[3] = rect.height;
}

template <class T>
Rect tldArrayToRect(T * boundary)
{
    Rect rect;
    rect.x = boundary[0];
    rect.y = boundary[1];
    rect.width = boundary[2];
    rect.height = boundary[3];

    return rect;
}


int tldIsInside(int * bb1, int * bb2);
void tldRectToPoints(CvRect rect, CvPoint * p1, CvPoint * p2);
void tldBoundingBoxToPoints(int * bb, CvPoint * p1, CvPoint * p2);

void tldNormalizeImg(IplImage * img, float * result, int size);

void tldExtractNormalizedPatch(Mat img, int x, int y, int w, int h, float * output);
void tldExtractNormalizedPatchBB(Mat img, int * boundary, float * output);
void tldExtractNormalizedPatchRect(Mat img, Rect* rect, float * output);
IplImage * tldExtractSubImage(IplImage * img, int * boundary);
IplImage * tldExtractSubImage(IplImage * img, int x, int y, int w, int h);

float tldCalcMean(float * value, int n);
float tldCalcVariance(float * value, int n);

bool tldSortByOverlapDesc(pair<int, float> bb1 , pair<int, float> bb2);
Rect* tldCopyRect(Rect* r);

//TODO: Change function names
float tldOverlapRectRect(Rect r1, Rect r2);
void tldOverlapOne(int * windows, int numWindows, int index, vector<int> * indices, float * overlap);
void tldOverlap(int * windows, int numWindows, int * boundary, float * overlap);
void tldOverlapRect(int * windows, int numWindows, Rect * boundary, float * overlap);

float tldCalcVariance(float * value, int n);

#endif /* UTIL_H_ */

} /* End Namespace */
