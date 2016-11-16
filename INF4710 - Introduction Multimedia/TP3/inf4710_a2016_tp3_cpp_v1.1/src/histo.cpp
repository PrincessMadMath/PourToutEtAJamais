#include "utils.hpp"

inline int getPartitionIndex(uchar value, size_t N)
{
	float intervalCount = 256.0 / (float) N;
	int index =  value / intervalCount;

	return index;
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

			int blue_index = getPartitionIndex(intensity.val[blue], N);
			int green_index = getPartitionIndex(intensity.val[green], N);
			int red_index = getPartitionIndex(intensity.val[red], N);

			try {
				++oHist.at<float>(blue, blue_index);
				++oHist.at<float>(green, green_index);
				++oHist.at<float>(red, red_index);
			}
			catch (...)
			{
				std::cout << "Shiiiiiiiiiit";
			}
			
		}
	}

    return oHist;
}
