
#pragma once

#include "tp2.h"

#define PI 3.141592653589793238462643383279502
#include <cmath>




inline int xy_2_index(int xy) {
	return xy - 1;
}

inline int uv_2_index(int uv) {
	return uv - 1;
}


inline float getC(int w, int n)
{
	if (w == 1)
	{
		return std::sqrt(1.0 / n);
	}
	else 
	{
		return std::sqrt(2.0 / n);
	}
}

// dct: computes the discrete cosinus tranform of a matrix
template<typename Tin=uchar,typename Tout=float>
inline cv::Mat_<Tout> dct(const cv::Mat_<Tin>& oBlock) {
    CV_Assert(!oBlock.empty() && oBlock.rows==oBlock.cols && (oBlock.rows%2)==0 && oBlock.isContinuous());
    cv::Mat_<Tout> oOutput(oBlock.size());

	int n = oBlock.rows;

	for (int u = 1; u <= n; ++u)
	{
		for (int v = 1; v <= n; ++v)
		{
			// For C(u,v)

			// c(u) * c(v)
			float factor = getC(u, n) * getC(v, n);
			float sum = 0.0;
			for (int x = 1; x <= n; ++x) {
				for (int y = 1; y <= n; ++y) {
					Tin value = oBlock(xy_2_index(x), xy_2_index(y));
					float insideCos1 = (PI*(2 * (x - 1) + 1)*(u - 1)) / (2 * n);
					float insideCos2 = (PI*(2 * (y - 1) + 1)*(v - 1)) / (2 * n);
					sum += value * cos(insideCos1) * cos(insideCos2);
				}
			}
			float dct_transform = factor * sum;

			oOutput(uv_2_index(u), uv_2_index(v)) = dct_transform;
		}
	}

    return oOutput;
}


