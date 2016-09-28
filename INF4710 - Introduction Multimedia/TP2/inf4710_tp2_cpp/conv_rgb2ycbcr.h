
#pragma once

#include "tp2.h"
#include <opencv\cv.h>

#define blue 0
#define green 1
#define red 2


inline uchar getY(cv::Vec3b rbg_intensity)
{
	uchar Y = 0.299*rbg_intensity.val[red] + 0.587*rbg_intensity.val[green] + 0.114*rbg_intensity.val[blue];
	return Y;
}

inline uchar getCb(cv::Vec3b rbg_intensity, uchar Y)
{
	uchar Cb = 128 + 0.564*(0.114*rbg_intensity.val[blue] - Y);
	return Cb;
}

inline uchar getCr(cv::Vec3b rbg_intensity, uchar Y)
{
	uchar Cr = 128 + 0.713*(0.114*rbg_intensity.val[red] - Y);
	return Cr;
}


// conv_rgb2ycbcr: converts an 8-bit-depth RGB image to Y'CbCr format using optional 4:2:0 subsampling
inline void conv_rgb2ycbcr(const cv::Mat& RGB, bool bSubsample, cv::Mat_<uchar>& Y, cv::Mat_<uchar>& Cb, cv::Mat_<uchar>& Cr) {
    CV_Assert(!RGB.empty() && RGB.type()==CV_8UC3 && RGB.isContinuous());
    Y.create(RGB.rows,RGB.cols);

	int sampling_rows = bSubsample ? RGB.rows / 2 : RGB.rows;
	int sampling_cols = bSubsample ? RGB.cols / 2 : RGB.cols;
	Cb.create(sampling_rows, sampling_cols);
    Cr.create(sampling_rows, sampling_cols);

	for (int row_index = 0; row_index < RGB.rows; ++row_index)
	{
		for (int col_index = 0; col_index < RGB.cols; ++col_index)
		{
			cv::Vec3b intensity = RGB.at<cv::Vec3b>(row_index, col_index);

			uchar y = getY(intensity);
			uchar cb = getCb(intensity, y);
			uchar cr = getCr(intensity, y);

			// We always sample Y
			Y(row_index, col_index) = y;

			if (bSubsample) {
				// Sub-sampling for 4:2:0
				if (col_index % 2 == 0 && row_index % 2 == 0)
				{
					int row_adjusted = row_index / 2;
					int col_adjusted = col_index / 2;

					Cb(row_adjusted, col_adjusted) = cb;
					Cr(row_adjusted, col_adjusted) = cr;
				}
			}
			else {
				Cb(row_index, col_index) = cb;
				Cr(row_index, col_index) = cr;
			}

		}
	}
}

