
#pragma once

#include "tp2.h"

inline uchar getR(uchar y, uchar cr) {
	uchar r = y + 1.403*(cr - 128);
	return r;
}

inline uchar getG(uchar y, uchar cr, uchar cb) {
	uchar g = y - 0.714*(cr - 128) - 0.344*(cb - 128);
	return g;
}

inline uchar getB(uchar y, uchar cb) {
	uchar b = y + 1.773*(cb - 128);
	return b;
}


// conv_ycbcr2rgb: converts an 8-bit-depth YCbCr image to RGB format considering optional 4:2:0 subsampling
inline void conv_ycbcr2rgb(const cv::Mat_<uchar>& Y, const cv::Mat_<uchar>& Cb, const cv::Mat_<uchar>& Cr, bool bSubsample, cv::Mat& RGB) {
    CV_Assert(!Y.empty() && Y.isContinuous() && !Cb.empty() && Cb.isContinuous() && !Cr.empty() && Cr.isContinuous());
    CV_Assert(!bSubsample || (Y.rows/2==Cb.rows && Y.rows/2==Cr.rows && Y.cols/2==Cb.cols && Y.cols/2==Cr.cols));
    CV_Assert(bSubsample || (Y.rows==Cb.rows && Y.rows==Cr.rows && Y.cols==Cb.cols && Y.cols==Cr.cols));
    RGB.create(Y.size(),CV_8UC3);

	for (int row_index = 0; row_index < RGB.rows; ++row_index)
	{
		for (int col_index = 0; col_index < RGB.cols; ++col_index)
		{
			uchar y = Y(row_index, col_index);
			uchar cb = 0;
			uchar cr = 0;

			if (bSubsample) 
			{
				int row_adjusted =  row_index / 2;
				int col_adjusted =  col_index / 2;

				cb = Cb(row_adjusted, col_adjusted);
				cr = Cr(row_adjusted, col_adjusted);
			}
			else {
				cb = Cb(row_index, col_index);
				cr = Cr(row_index, col_index);
			}

			uchar b = getB(y, cb);
			uchar r = getR(y, cr);
			uchar g = getG(y, cr, cb);

			cv::Vec3b rgb_intensity = cv::Vec3b(b, g, r);

			RGB.at<cv::Vec3b>(row_index, col_index) = rgb_intensity;
		}
	}
}


