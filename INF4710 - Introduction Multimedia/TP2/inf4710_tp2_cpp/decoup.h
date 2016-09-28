
#pragma once

#include "tp2.h"

// decoup: reformats a 2D image (i.e. always single channel) to a block array
template<size_t nBlockSize=8, typename T=uchar>
inline std::vector<cv::Mat_<T>> decoup(const cv::Mat_<T>& oImage) {
    CV_Assert(!oImage.empty() && (oImage.rows%nBlockSize)==0 && (oImage.cols%nBlockSize)==0 && oImage.isContinuous());
    std::vector<cv::Mat_<T>> vOutput;

    // @@@@ TODO

    return vOutput;
}
