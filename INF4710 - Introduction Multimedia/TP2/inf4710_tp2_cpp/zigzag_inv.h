
#pragma once

#include "tp2.h"

// zigzag_inv: returns an NxN block created by de-zigzagging through a (NxN)-element 1D array
template<int nBlockSize=8, typename T=short>
inline cv::Mat_<T> zigzag_inv(const std::array<T,nBlockSize*nBlockSize>& aZigzag) {
    int nIdx = 0;
    cv::Mat_<T> oOutput(nBlockSize*nBlockSize,1);
    for(int i=0; i<nBlockSize*2; ++i)
        for(int j=(i<nBlockSize)?0:i-nBlockSize+1; j<=i && j<nBlockSize; ++j)
            oOutput((i&1)?j*(nBlockSize-1)+i:(i-j)*nBlockSize+j) = aZigzag[nIdx++];
    return oOutput.reshape(0,nBlockSize);
}
