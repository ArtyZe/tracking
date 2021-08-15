/*
 * MedianFlowTracker.h
 *
 *  Created on: Nov 17, 2011
 *      Author: Georg Nebehay
 */

#ifndef MEDIANFLOWTRACKER_H_
#define MEDIANFLOWTRACKER_H_

#include <opencv/cv.h>

using namespace cv;

namespace tld
{

class MedianFlowTracker
{
public:
    Rect* trackerBB;

    MedianFlowTracker();
    virtual ~MedianFlowTracker();
    void cleanPreviousData();
    void track(Mat prevImg, Mat currImg, Rect* prevBB);
};

} /* namespace tld */
#endif /* MEDIANFLOWTRACKER_H_ */
