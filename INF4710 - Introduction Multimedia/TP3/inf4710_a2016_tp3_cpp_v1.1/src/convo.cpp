#include "utils.hpp"

cv::Mat tp3::convo(const cv::Mat& oImage, const cv::Mat_<float>& oKernel) {
    CV_Assert(!oImage.empty() && oImage.type()==CV_8UC3 && oImage.isContinuous());
    CV_Assert(!oKernel.empty() && oKernel.cols==oKernel.rows && oKernel.isContinuous());
    CV_Assert(oImage.cols>oKernel.cols && oImage.rows>oKernel.rows);
    cv::Mat oResult(oImage.size(),CV_32FC1);

    // @@@@ TODO

    return oResult;
}


inline cv::Vec3b GetIntensity(const cv::Mat& image, int index_x, int index_y) {


}

inline int middle(int min, int value, int max)
{
	if (value < min) {
		return min;
	}
	else if (value > max)
	{
		return max;
	}
	return value;
}