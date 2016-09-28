
#pragma once

#include "tp2.h"

// zigzag: returns a (NxN)-element 1D array created by zig-zagging through a NxN block
template<int nBlockSize=8, typename T=short>
inline std::array<T,nBlockSize*nBlockSize> zigzag(const cv::Mat_<T>& oBlock) {
	CV_Assert(!oBlock.empty() && oBlock.rows==oBlock.cols && oBlock.rows==nBlockSize);
    std::array<T,nBlockSize*nBlockSize> aZigzag;

    // @@@@ TODO
	
	return aZigzag;
}
