
#pragma once

#include "tp2.h"
#include <cmath>


#define PI 3.141592653589793238462643383279502


// dct_inv: computes the inverse discrete cosinus tranform of a matrix
template<typename Tin=float,typename Tout=uchar>
inline cv::Mat_<Tout> dct_inv(const cv::Mat_<Tin>& oBlock) {
    CV_Assert(!oBlock.empty() && oBlock.rows==oBlock.cols && (oBlock.rows%2)==0 && oBlock.isContinuous());
    cv::Mat_<Tout> oOutput(oBlock.size());

	int n = oBlock.rows;

	for (int x = 1; x <= n; ++x)
	{
		for (int y = 1; y <= n; ++y)
		{

			float sum = 0.0;
			for (int u = 1; u <= n; ++u) {
				for (int v = 1; v <= n; ++v) {

					// c(u) * c(v)
					float factor = getC(u, n) * getC(v, n);

					Tin value = oBlock(uv_2_index(u), uv_2_index(v));

					float insideCos1 = (PI*(2 * (x - 1) + 1)*(u - 1)) / (2 * n);
					float insideCos2 = (PI*(2 * (y - 1) + 1)*(v - 1)) / (2 * n);

					sum += factor * value * cos(insideCos1) * cos(insideCos2);
				}
			}

			oOutput(xy_2_index(x), xy_2_index(y)) = sum;
		}
	}

	return oOutput;
}
