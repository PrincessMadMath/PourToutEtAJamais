#include "utils.hpp"

inline int getPartitionIndex(uchar value, size_t N)
{
	int intervalCount = 256 / N;
	return value / intervalCount;
}

cv::Mat tp3::histo(const cv::Mat& oImage, size_t N) {
    CV_Assert(!oImage.empty() && oImage.type()==CV_8UC3 && oImage.isContinuous());
    CV_Assert(N>1 && N<=256);
    cv::Mat oHist(3,(int)N,CV_32FC1,cv::Scalar_<float>(0.0f));

	for (int row_index = 0; row_index < oImage.rows; ++row_index)
	{
		for (int col_index = 0; col_index < oImage.cols; ++col_index)
		{
			cv::Vec3b intensity = oImage.at<cv::Vec3b>(row_index, col_index);

			++oHist.at<float>(blue, getPartitionIndex(intensity.val[blue], N));
			++oHist.at<float>(green, getPartitionIndex(intensity.val[green], N));
			++oHist.at<float>(red, getPartitionIndex(intensity.val[red], N));
		}
	}

    return oHist;
}
