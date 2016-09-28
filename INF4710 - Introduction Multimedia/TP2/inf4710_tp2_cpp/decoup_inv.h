
#pragma once

#include "tp2.h"

// decoup_inv: reconstructs a 2D image (i.e. always single channel) from a block array
template<size_t nBlockSize=8, typename T=uchar>
inline cv::Mat_<T> decoup_inv(const std::vector<cv::Mat_<T>>& vBlocks, const cv::Size& oImageSize) {
    CV_Assert(!vBlocks.empty() && !vBlocks[0].empty() && vBlocks[0].rows==nBlockSize && vBlocks[0].cols==nBlockSize && vBlocks[0].isContinuous());
    CV_Assert(oImageSize.area()>0 && (oImageSize.height%nBlockSize)==0 && (oImageSize.width%nBlockSize)==0);
    cv::Mat_<T> oOutput(oImageSize);

    // @@@@ TODO

    return oOutput;
}
