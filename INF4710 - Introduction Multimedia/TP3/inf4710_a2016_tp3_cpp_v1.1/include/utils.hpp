#pragma once 

#include <cmath>
#include <vector>
#include <iomanip>
#include <fstream>
#include <map>
#include <iostream>
#include <queue>
#include <map>
#include <array>
#include <climits>
#include <iterator>
#include <algorithm>
#include <type_traits>
#include <numeric>
#include <iostream>
#include <opencv2/opencv.hpp>

#include <opencv\cv.h>
#define blue 0
#define green 1
#define red 2



namespace tp3 {

    cv::Mat histo(const cv::Mat& image, size_t N);
    cv::Mat convo(const cv::Mat& image, const cv::Mat_<float>& kernel);

} // unnamed namespace