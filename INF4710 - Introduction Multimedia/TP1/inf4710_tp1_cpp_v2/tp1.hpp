
//
// INF4710 TP1 A2016 v1.0
// Auteurs:
//     @@@@@@@@@@@@@@@@@@@@@@@
//     @@@@@@@@@@@@@@@@@@@@@@@
//

#pragma once

#include <exception>
#include <iostream>
#include <string>
#include <opencv2/opencv.hpp>

struct LZ77Code {
    uint8_t nIdx; // in-dictionary index offset for first match
    uint8_t nLength; // total consecutive match count
    uint8_t nNextSymbol; // next unmatched symbol
    inline std::string to_string() const {
        return std::string("(")+std::to_string(nIdx)+","+std::to_string(nLength)+",'"+std::to_string(nNextSymbol)+"'),";
    }
};

inline std::vector<LZ77Code> lz77_encode(const std::vector<uint8_t>& vSignal, size_t N, size_t n1) {
    CV_Assert(N>0 && n1>0 && N>n1 && n1<UCHAR_MAX && !vSignal.empty());
    std::vector<LZ77Code> vCode;
    // ... @@@@@ TODO (encode vSignal using lz77, and put triplets in vCode)
    return vCode;
}

inline std::vector<uint8_t> lz77_decode(const std::vector<LZ77Code>& vCode, size_t N, size_t n1) {
    CV_Assert(N>0 && n1>0 && N>n1 && n1<UCHAR_MAX && !vCode.empty());
    std::vector<uint8_t> vSignal;
    // ... @@@@@ TODO (decode vCode triplets using lz77, and put original values in vSignal)
    return vSignal;
}

inline std::vector<uint8_t> format_signal(const cv::Mat& oInputImage) {
    CV_Assert(!oInputImage.empty() && oInputImage.isContinuous() && (oInputImage.type()==CV_8UC1 || oInputImage.type()==CV_8UC3));
    std::vector<uint8_t> vSignal;
    // ... @@@@@ TODO (put oInputImage data in vSignal in correct order/format)
    return vSignal;
}

inline cv::Mat reformat_image(const std::vector<uint8_t>& vSignal, const cv::Size& oOrigImageSize) {
    CV_Assert(!vSignal.empty() && oOrigImageSize.area()>0 && (oOrigImageSize.area()==vSignal.size() || oOrigImageSize.area()*3==vSignal.size())); // output can only be 1-channel (CV_8UC1) or 3-channel (CV_8UC3)
    if(oOrigImageSize.area()==vSignal.size()) // if the image is single-channel (CV_8UC1), reassemble inline as cv::Mat data is row-major
        return cv::Mat(oOrigImageSize,CV_8UC1,(void*)vSignal.data()).clone();
    std::vector<cv::Mat> voChannels(3); // otherwise, use cv::merge to reinterlace individual CV_8UC1 matrices into a CV_8UC3 one
    for(size_t c=0; c<3; ++c)
        voChannels[c] = cv::Mat(oOrigImageSize,CV_8UC1,(void*)(vSignal.data()+c*oOrigImageSize.area())); // no need for a clone here, we will not modify the original data
    cv::Mat oOrigImage; // will be created/sized automatically by cv::merge
    cv::merge(voChannels,oOrigImage);
    return oOrigImage;
}
