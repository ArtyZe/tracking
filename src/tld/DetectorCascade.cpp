/*
 * DetectorCascade.cpp
 *
 *  Created on: Nov 16, 2011
 *      Author: Georg Nebehay
 */

#include "DetectorCascade.h"
#include <algorithm>
#include "TLDUtil.h"

namespace tld
{

//TODO: Convert this to a function
#define sub2idx(x,y,imgWidthStep) ((int) (floor((x)+0.5) + floor((y)+0.5)*(imgWidthStep)))

DetectorCascade::DetectorCascade()
{
    objWidth = -1; //MUST be set before calling init
    objHeight = -1; //MUST be set before calling init
    useShift = 1;
    imgHeight = -1;
    imgWidth = -1;

    shift = 0.1;
    minScale = -10;
    maxScale = 10;
    minSize = 25;
    imgWidthStep = -1;

    numTrees = 13;
    numFeatures = 10;
    initialised = false;

    foregroundDetector = new ForegroundDetector();
    varianceFilter = new VarianceFilter();
    ensembleClassifier = new EnsembleClassifier();
    nnClassifier = new NNClassifier();
    clustering = new Clustering();
    detectionResult = new DetectionResult();
}

DetectorCascade::~DetectorCascade()
{
    release();

    delete foregroundDetector;
    delete varianceFilter;
    delete ensembleClassifier;
    delete nnClassifier;
    delete detectionResult;
}

void DetectorCascade::init()
{
    if (imgWidth == -1 || imgHeight == -1 || imgWidthStep == -1 || objWidth == -1 || objHeight == -1)
    {
        //printf("Error: Window dimensions not set\n"); //TODO: Convert this to exception
    }

    initWindowsAndScales();
    initWindowOffsets();

    propagateMembers();

    ensembleClassifier->init();

    initialised = true;
}

//TODO: This is error-prone. Better give components a reference to DetectorCascade?
void DetectorCascade::propagateMembers()
{
    detectionResult->init(numWindows, numTrees);

    varianceFilter->windowOffsets = windowOffsets;
    ensembleClassifier->windowOffsets = windowOffsets;
    ensembleClassifier->imgWidthStep = imgWidthStep;
    ensembleClassifier->numScales = numScales;
    ensembleClassifier->scales = scales;
    ensembleClassifier->numFeatures = numFeatures;
    ensembleClassifier->numTrees = numTrees;
    nnClassifier->windows = windows;
    clustering->windows = windows;
    clustering->numWindows = numWindows;

    foregroundDetector->minBlobSize = minSize * minSize;

    foregroundDetector->detectionResult = detectionResult;
    varianceFilter->detectionResult = detectionResult;
    ensembleClassifier->detectionResult = detectionResult;
    nnClassifier->detectionResult = detectionResult;
    clustering->detectionResult = detectionResult;
}

void DetectorCascade::release()
{
    if (!initialised)
    {
        return; //Do nothing
    }

    initialised = false;

    foregroundDetector->release();
    ensembleClassifier->release();
    nnClassifier->release();

    clustering->release();

    numWindows = 0;
    numScales = 0;

    delete[] scales;
    delete[] windows;
    delete[] windowOffsets;

    objWidth = -1;
    objHeight = -1;

    detectionResult->release();
}

void DetectorCascade::cleanPreviousData()
{
    detectionResult->reset();
}

/* returns number of bounding boxes, bounding boxes, number of scales, scales
 * bounding boxes are stored in an array of size 5*numBBs using the format <x y w h scaleIndex>
 * scales are stored using the format <w h>
 *
 */
void DetectorCascade::initWindowsAndScales()
{
    int scanAreaX = 1; // It is important to start with 1/1, because the integral images aren't defined at pos(-1,-1) due to speed reasons
    int scanAreaY = 1;
    int scanAreaW = imgWidth - 1;
    int scanAreaH = imgHeight - 1;

    int windowIndex = 0;

    scales = new Size[maxScale - minScale + 1];

    numWindows = 0;

    int scaleIndex = 0;

    for (int i = minScale; i <= maxScale; i++)
    {
        float scale = pow(1.2, i);
        int w = (int)objWidth * scale;
        int h = (int)objHeight * scale;
        int ssw, ssh;

        if (useShift)
        {
            ssw = max<float>(1, w * shift);
            ssh = max<float>(1, h * shift);
        }
        else
        {
            ssw = 1;
            ssh = 1;
        }

        if (w < minSize || h < minSize || w > scanAreaW || h > scanAreaH)
        {
            continue;
        }

        scales[scaleIndex].width = w;
        scales[scaleIndex].height = h;

        scaleIndex++;

        numWindows += floor((float)(scanAreaW - w + ssw) / ssw) * floor((float)(scanAreaH - h + ssh) / ssh);
    }

    numScales = scaleIndex;

    windows = new int[TLD_WINDOW_SIZE * numWindows];

    for (scaleIndex = 0; scaleIndex < numScales; scaleIndex++)
    {
        int w = scales[scaleIndex].width;
        int h = scales[scaleIndex].height;

        int ssw, ssh;

        if (useShift)
        {
            ssw = max<float>(1, w * shift);
            ssh = max<float>(1, h * shift);
        }
        else
        {
            ssw = 1;
            ssh = 1;
        }

        for (int y = scanAreaY; y + h <= scanAreaY + scanAreaH; y += ssh)
        {
            for (int x = scanAreaX; x + w <= scanAreaX + scanAreaW; x += ssw)
            {
                int * bb = &windows[TLD_WINDOW_SIZE * windowIndex];
                tldCopyBoundaryToArray<int>(x, y, w, h, bb);
                bb[4] = scaleIndex;

                windowIndex++;
            }
        }
    }

    assert(windowIndex == numWindows);
}

//Creates offsets that can be added to bounding boxes
//offsets are contained in the form delta11, delta12,... (combined index of dw and dh)
//Order: scale->tree->feature
void DetectorCascade::initWindowOffsets()
{
    windowOffsets = new int[TLD_WINDOW_OFFSET_SIZE * numWindows];
    int *off = windowOffsets;

    int windowSize = TLD_WINDOW_SIZE;

    for (int i = 0; i < numWindows; i++)
    {
        int *window = windows + windowSize * i;

        *off++ = sub2idx(window[0] - 1, window[1] - 1, imgWidthStep); // x1-1,y1-1
        *off++ = sub2idx(window[0] - 1, window[1] + window[3] - 1, imgWidthStep); // x1-1,y2
        *off++ = sub2idx(window[0] + window[2] - 1, window[1] - 1, imgWidthStep); // x2,y1-1
        *off++ = sub2idx(window[0] + window[2] - 1, window[1] + window[3] - 1, imgWidthStep); // x2,y2
        *off++ = window[4] * 2 * numFeatures * numTrees; // pointer to features for this scale
        *off++ = window[2] * window[3]; //Area of bounding box
    }
}

void DetectorCascade::detect(Mat img)
{
    //For every bounding box, the output is confidence, pattern, variance
    detectionResult->reset();

    if (!initialised)
    {
        return;
    }

    //Prepare components
    foregroundDetector->nextIteration(img); //Calculates foreground
    varianceFilter->nextIteration(img); //Calculates integral images
    ensembleClassifier->nextIteration(img);

    if (!varianceFilter->enabled &&
        !ensembleClassifier->enabled &&
        !nnClassifier->enabled &&
        !foregroundDetector->isActive())
    {
        return;
    }

    #pragma omp parallel for

    for (int i = 0; i < numWindows; i++)
    {
        int * window = &windows[TLD_WINDOW_SIZE * i];

        if (foregroundDetector->isActive())
        {
            bool isInside = false;

            for (size_t j = 0; j < detectionResult->fgList->size(); j++)
            {
                int bgBox[4];

                tldRectToArray(detectionResult->fgList->at(j), bgBox);

                if (tldIsInside(window, bgBox)) //TODO: This is inefficient and should be replaced by a quadtree
                {
                    isInside = true;
                }
            }

            if (!isInside)
            {
                detectionResult->posteriors[i] = 0;
                continue;
            }
        }

        if (!varianceFilter->filter(i))
        {
            detectionResult->posteriors[i] = 0;
            continue;
        }

        if (!ensembleClassifier->filter(i))
        {
            continue;
        }

        if (!nnClassifier->filter(img, i))
        {
            continue;
        }

        detectionResult->confidentIndices->push_back(i);
    }

    //Cluster
    clustering->clusterConfidentIndices();

    detectionResult->containsValidData = true;
}

} /* namespace tld */
