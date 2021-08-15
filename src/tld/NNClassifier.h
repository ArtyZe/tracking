/*
 * NNClassifier.h
 *
 *  Created on: Nov 16, 2011
 *      Author: Georg Nebehay
 */

#ifndef NNCLASSIFIER_H_
#define NNCLASSIFIER_H_

#include <vector>
#include <opencv/cv.h>

#include "NormalizedPatch.h"
#include "DetectionResult.h"

using namespace std;
using namespace cv;

namespace tld
{

class NNClassifier
{
    float ncc(float *f1, float *f2);
public:
    bool enabled;

    int * windows;
    float thetaFP;
    float thetaTP;
    DetectionResult* detectionResult;
    vector<NormalizedPatch>* falsePositives;
    vector<NormalizedPatch>* truePositives;

    NNClassifier();
    virtual ~NNClassifier();

    void release();
    float classifyPatch(NormalizedPatch * patch);
    float classifyBB(Mat img, Rect* bb);
    float classifyWindow(Mat img, int windowIdx);
    void learn(vector<NormalizedPatch> patches);
    bool filter(Mat img, int windowIdx);
};

} /* namespace tld */
#endif /* NNCLASSIFIER_H_ */
