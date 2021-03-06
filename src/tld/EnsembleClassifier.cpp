/*
 * EnsembleClassifier.cpp
 *
 *  Created on: Nov 16, 2011
 *      Author: Georg Nebehay
 */
#include <cstdlib>
#include <math.h>
#include <opencv/cv.h>
#include "DetectorCascade.h"
#include "EnsembleClassifier.h"

using namespace std;
using namespace cv;

namespace tld
{

//TODO: Convert this to a function
#define sub2idx(x,y,widthstep) ((int) (floor((x)+0.5) + floor((y)+0.5)*(widthstep)))

EnsembleClassifier::EnsembleClassifier()
{
    numTrees = 10;
    numFeatures = 13;
    enabled = true;
}

EnsembleClassifier::~EnsembleClassifier()
{
    release();
}

void EnsembleClassifier::init()
{
    numIndices = pow(2.0f, numFeatures);

    initFeatureLocations();
    initFeatureOffsets();
    initPosteriors();
}

void EnsembleClassifier::release()
{
    delete[] features;
    features = NULL;
    delete[] featureOffsets;
    featureOffsets = NULL;
    delete[] posteriors;
    posteriors = NULL;
    delete[] positives;
    positives = NULL;
    delete[] negatives;
    negatives = NULL;
}

/*
 * Generates random measurements in the format <x1,y1,x2,y2>
 */
void EnsembleClassifier::initFeatureLocations()
{
    int size = 2 * 2 * numFeatures * numTrees;

    features = new float[size];

    for (int i = 0; i < size; i++)
    {
        features[i] = rand() / (float)RAND_MAX;
    }
}

//Creates offsets that can be added to bounding boxes
//offsets are contained in the form delta11, delta12,... (combined index of dw and dh)
//Order: scale.tree->feature
void EnsembleClassifier::initFeatureOffsets()
{
    featureOffsets = new int[numScales * numTrees * numFeatures * 2];
    int *off = featureOffsets;

    for (int k = 0; k < numScales; k++)
    {
        Size scale = scales[k];

        for (int i = 0; i < numTrees; i++)
        {
            for (int j = 0; j < numFeatures; j++)
            {
                float *currentFeature  = features + (4 * numFeatures) * i + 4 * j;

                //We add +1 because the index of the bounding box points to x-1, y-1
                *off++ = sub2idx((scale.width - 1) * currentFeature[0] + 1, (scale.height - 1) * currentFeature[1] + 1, imgWidthStep);
                *off++ = sub2idx((scale.width - 1) * currentFeature[2] + 1, (scale.height - 1) * currentFeature[3] + 1, imgWidthStep);
            }
        }
    }
}

void EnsembleClassifier::initPosteriors()
{
    posteriors = new float[numTrees * numIndices];
    positives = new int[numTrees * numIndices];
    negatives = new int[numTrees * numIndices];

    for (int i = 0; i < numTrees; i++)
    {
        for (int j = 0; j < numIndices; j++)
        {
            posteriors[i * numIndices + j] = 0;
            positives[i * numIndices + j] = 0;
            negatives[i * numIndices + j] = 0;
        }
    }
}

void EnsembleClassifier::nextIteration(Mat img)
{
    if (!enabled)
    {
        return;
    }

    this->img = (unsigned char *)img.data;
}

//Classical fern algorithm
int EnsembleClassifier::calcFernFeature(int windowIdx, int treeIdx)
{
    int index = 0;
    int *bbox = windowOffsets + windowIdx * TLD_WINDOW_OFFSET_SIZE;
    //bbox[4] is pointer to features for the current scale
    int *off = featureOffsets + bbox[4] + treeIdx * 2 * numFeatures;

    for (int i = 0; i < numFeatures; i++)
    {
        index <<= 1;

        int fp0 = img[bbox[0] + off[0]];
        int fp1 = img[bbox[0] + off[1]];

        if (fp0 > fp1)
        {
            index |= 1;
        }

        off += 2;
    }

    return index;
}

void EnsembleClassifier::calcFeatureVector(int windowIdx, int * featureVector)
{
    for (int i = 0; i < numTrees; i++)
    {
        featureVector[i] = calcFernFeature(windowIdx, i);
    }
}

float EnsembleClassifier::calcConfidence(int * featureVector)
{
    float conf = 0.0;

    for (int i = 0; i < numTrees; i++)
    {
        conf += posteriors[i * numIndices + featureVector[i]];
    }

    return conf;
}

void EnsembleClassifier::classifyWindow(int windowIdx)
{
    int* featureVector = detectionResult->featureVectors + numTrees * windowIdx;

    calcFeatureVector(windowIdx, featureVector);

    detectionResult->posteriors[windowIdx] = calcConfidence(featureVector);
}

bool EnsembleClassifier::filter(int i)
{
    if (!enabled)
    {
        return true;
    }

    classifyWindow(i);

    if (detectionResult->posteriors[i] < 0.5)
    {
        return false;
    }

    return true;
}

void EnsembleClassifier::updatePosterior(int treeIdx, int idx, int positive, int amount)
{
    int arrayIndex = treeIdx * numIndices + idx;

    (positive) ? positives[arrayIndex] += amount : negatives[arrayIndex] += amount;
    posteriors[arrayIndex] = ((float)positives[arrayIndex]) / (positives[arrayIndex] + negatives[arrayIndex]) / 10.0;
}

void EnsembleClassifier::updatePosteriors(int *featureVector, int positive, int amount)
{
    for (int i = 0; i < numTrees; i++)
    {
        int idx = featureVector[i];
        updatePosterior(i, idx, positive, amount);
    }
}

void EnsembleClassifier::learn(Mat img, int * boundary, int positive, int * featureVector)
{
    if (!enabled)
    {
        return;
    }

    float conf = calcConfidence(featureVector);

    //Update if positive patch and confidence < 0.5 or negative and conf > 0.5
    if ((positive && conf < 0.5) || (!positive && conf > 0.5))
    {
        updatePosteriors(featureVector, positive, 1);
    }
}

} /* namespace tld */
