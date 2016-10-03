
#pragma once

#include "tp2.h"

// decoup: reformats a 2D image (i.e. always single channel) to a block array
template<size_t nBlockSize=8, typename T=uchar>
inline std::vector<cv::Mat_<T>> decoup(const cv::Mat_<T>& oImage) {
    CV_Assert(!oImage.empty() && (oImage.rows%nBlockSize)==0 && (oImage.cols%nBlockSize)==0 && oImage.isContinuous());
    std::vector<cv::Mat_<T>> vOutput;

	for (int block_row_index = 0; block_row_index < oImage.rows / nBlockSize; ++block_row_index)
	{
		for (int block_col_index = 0; block_col_index < oImage.cols / nBlockSize; ++block_col_index)
		{
			cv::Mat_<T> block(nBlockSize, nBlockSize);
			for (int row_index = 0; row_index < nBlockSize; ++row_index)
			{
				for (int col_index = 0; col_index < nBlockSize; ++col_index)
				{
					int image_row_index = block_row_index * nBlockSize + row_index;
					int image_col_index = block_col_index * nBlockSize + col_index;
					block(row_index, col_index) = oImage.at<T>(image_row_index, image_col_index);
				}
			}
			vOutput.push_back(block);
		}
	}

    return vOutput;
}
