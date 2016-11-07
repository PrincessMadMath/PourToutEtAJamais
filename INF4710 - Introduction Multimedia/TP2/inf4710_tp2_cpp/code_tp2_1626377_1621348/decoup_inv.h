
#pragma once

#include "tp2.h"

// decoup_inv: reconstructs a 2D image (i.e. always single channel) from a block array
template<size_t nBlockSize=8, typename T=uchar>
inline cv::Mat_<T> decoup_inv(const std::vector<cv::Mat_<T>>& vBlocks, const cv::Size& oImageSize) {
    CV_Assert(!vBlocks.empty() && !vBlocks[0].empty() && vBlocks[0].rows==nBlockSize && vBlocks[0].cols==nBlockSize && vBlocks[0].isContinuous());
    CV_Assert(oImageSize.area()>0 && (oImageSize.height%nBlockSize)==0 && (oImageSize.width%nBlockSize)==0);
    cv::Mat_<T> oOutput(oImageSize);

	int nb_block_by_column = oImageSize.width / nBlockSize;

	for (int block_index = 0; block_index < vBlocks.size(); ++block_index)
	{
		for (int block_row_index = 0; block_row_index < nBlockSize; ++block_row_index)
		{
			for (int block_col_index = 0; block_col_index < nBlockSize; ++block_col_index)
			{
				int image_row_index = (block_index / nb_block_by_column) * nBlockSize + block_row_index;
				int image_col_index = (block_index % nb_block_by_column) * nBlockSize + block_col_index;

				try {
					oOutput(image_row_index, image_col_index) = vBlocks[block_index].at<T>(block_row_index, block_col_index);
				}
				catch (std::exception& e) {
					std::cout << e.what();
				}
			}
		}
	}

    return oOutput;
}
