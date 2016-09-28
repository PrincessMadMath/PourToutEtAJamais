
#pragma once

#include "tp2.h"

// conv_ycbcr2rgb: converts an 8-bit-depth YCbCr image to RGB format considering optional 4:2:0 subsampling
inline void conv_ycbcr2rgb(const cv::Mat_<uchar>& Y, const cv::Mat_<uchar>& Cb, const cv::Mat_<uchar>& Cr, bool bSubsample, cv::Mat& RGB) {
    CV_Assert(!Y.empty() && Y.isContinuous() && !Cb.empty() && Cb.isContinuous() && !Cr.empty() && Cr.isContinuous());
    CV_Assert(!bSubsample || (Y.rows/2==Cb.rows && Y.rows/2==Cr.rows && Y.cols/2==Cb.cols && Y.cols/2==Cr.cols));
    CV_Assert(bSubsample || (Y.rows==Cb.rows && Y.rows==Cr.rows && Y.cols==Cb.cols && Y.cols==Cr.cols));
    RGB.create(Y.size(),CV_8UC3);

    // @@@@ TODO

}
