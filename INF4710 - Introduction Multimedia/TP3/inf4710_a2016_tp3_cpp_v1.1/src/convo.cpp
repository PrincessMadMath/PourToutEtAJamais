#include "utils.hpp"


inline int middle(int min, int value, int max)
{
	if (value < min) {
		return min;
	}
	else if (value > max - 1)
	{
		return max - 1;
	}
	return value;
}

inline cv::Vec3b GetIntensity(const cv::Mat& image, int index_x, int index_y)
{
	int row_index = middle(0, index_x, image.rows);
	int col_index = middle(0, index_y, image.cols);

	try {

		return image.at<cv::Vec3b>(row_index, col_index);

	}
	catch (...)
	{
		std::cout << "Go look at the unicorn";
	}
}


float calculate_convolution(const cv::Mat& oImage, const cv::Mat_<float>& oKernel, int color, int row_index, int col_index)
{
	float sum = 0;

	int row_shift = -(oKernel.rows / 2);
	int col_shift = - (oKernel.cols / 2);
	
	for (int kernel_row_index = 0; kernel_row_index < oKernel.rows; ++kernel_row_index)
	{
		int image_row_index = row_index + row_shift + kernel_row_index;

		for (int kernel_col_index = 0; kernel_col_index < oKernel.cols; ++kernel_col_index)
		{	
				int image_col_index = col_index + col_shift + kernel_col_index;
				cv::Vec3b intensity = GetIntensity(oImage, image_row_index, image_col_index);

				float filterValue = oKernel.at<float>(kernel_row_index, kernel_col_index);

				sum += intensity.val[color] * filterValue;

			

		}
	}

	return sum;
}

cv::Mat tp3::convo(const cv::Mat& oImage, const cv::Mat_<float>& oKernel) {
    CV_Assert(!oImage.empty() && oImage.type()==CV_8UC3 && oImage.isContinuous());
    CV_Assert(!oKernel.empty() && oKernel.cols==oKernel.rows && oKernel.isContinuous());
    CV_Assert(oImage.cols>oKernel.cols && oImage.rows>oKernel.rows);
    cv::Mat oResult(oImage.size(),CV_32FC3);

		for (int row_index = 0; row_index < oImage.rows; ++row_index)
		{
			for (int col_index = 0; col_index < oImage.cols; ++col_index)
			{
				float resultBlue = calculate_convolution(oImage, oKernel, blue, row_index, col_index) / 255;
				float resultGreen = calculate_convolution(oImage, oKernel, green, row_index, col_index) / 255;
				float resultRed = calculate_convolution(oImage, oKernel, red, row_index, col_index) / 255;

				cv::Vec3f result = cv::Vec3f(resultBlue, resultGreen, resultRed);

				oResult.at<cv::Vec3f>(row_index, col_index) = result;
			}
		}

    return oResult;
}


