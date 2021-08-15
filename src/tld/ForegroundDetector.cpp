/*
 * ForegroundDetector.cpp
 *
 *  Created on: Nov 16, 2011
 *      Author: Georg Nebehay
 */

#include "ForegroundDetector.h"
#include "BlobResult.h"

using namespace cv;

namespace tld
{

ForegroundDetector::ForegroundDetector()
{
    fgThreshold = 16;
    minBlobSize = 0;
}

ForegroundDetector::~ForegroundDetector()
{
}

void ForegroundDetector::release()
{
}

void ForegroundDetector::nextIteration(Mat img)
{
    if (bgImg.empty())
    {
        return;
    }

    Mat absImg = Mat(img.cols, img.rows, img.type());
    Mat threshImg = Mat(img.cols, img.rows, img.type());

    absdiff(bgImg, img, absImg);
    threshold(absImg, threshImg, fgThreshold, 255, CV_THRESH_BINARY);

    IplImage im = (IplImage)threshImg;
    CBlobResult blobs = CBlobResult(&im, NULL, 0);

    blobs.Filter(blobs, B_EXCLUDE, CBlobGetArea(), B_LESS, minBlobSize);

    vector<Rect>* fgList = detectionResult->fgList;
    fgList->clear();

    for (int i = 0; i < blobs.GetNumBlobs(); i++)
    {
        CBlob * blob = blobs.GetBlob(i);
        CvRect rect = blob->GetBoundingBox();
        fgList->push_back(rect);
    }
}

bool ForegroundDetector::isActive()
{
    return !bgImg.empty();
}

} /* namespace tld */
