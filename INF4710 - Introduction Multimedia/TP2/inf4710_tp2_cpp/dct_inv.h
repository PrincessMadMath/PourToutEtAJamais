
#pragma once

#include "tp2.h"

// dct_inv: computes the inverse discrete cosinus tranform of a matrix
template<typename Tin=float,typename Tout=uchar>
inline cv::Mat_<Tout> dct_inv(const cv::Mat_<Tin>& oBlock) {
    CV_Assert(!oBlock.empty() && oBlock.rows==oBlock.cols && (oBlock.rows%2)==0 && oBlock.isContinuous());
    cv::Mat_<Tout> oOutput(oBlock.size());

    // @@@@ TODO

    return oOutput;
}
