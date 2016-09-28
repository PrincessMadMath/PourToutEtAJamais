
#pragma once

#include "tp2.h"

// dct: computes the discrete cosinus tranform of a matrix
template<typename Tin=uchar,typename Tout=float>
inline cv::Mat_<Tout> dct(const cv::Mat_<Tin>& oBlock) {
    CV_Assert(!oBlock.empty() && oBlock.rows==oBlock.cols && (oBlock.rows%2)==0 && oBlock.isContinuous());
    cv::Mat_<Tout> oOutput(oBlock.size());

    // @@@@ TODO

    return oOutput;
}
