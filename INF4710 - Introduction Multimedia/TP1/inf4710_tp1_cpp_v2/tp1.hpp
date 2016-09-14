
//
// INF4710 TP1 A2016 v1.0
// Auteurs:
//     Mathieu Gamache
//     Félix La Rocque Carrier
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

LZ77Code getCode(const std::vector<uint8_t>& vSignal, size_t encodeIndex, size_t N, size_t n1);
LZ77Code searchPartial(const std::vector<uint8_t>& vSignal, size_t encodeIndex, uint8_t currentOffset, uint8_t lowerOffset, uint8_t upperOffset);


inline std::vector<LZ77Code> lz77_encode(const std::vector<uint8_t>& vSignal, size_t N, size_t n1) {
    CV_Assert(N>0 && n1>0 && N>n1 && n1<UCHAR_MAX && !vSignal.empty());

    std::vector<LZ77Code> vCode;
	// ... @@@@@ TODO (encode vSignal using lz77, and put triplets in vCode)

	// For each symbol
	size_t encodeIndex = 0;
	while (encodeIndex < vSignal.size())
	{
		// Get LZ77 Code for current index
		LZ77Code currentCode = getCode(vSignal, encodeIndex, N, n1);

		vCode.push_back(currentCode);
		encodeIndex += currentCode.nLength + 1;
	}

    return vCode;
}

// Find the LZ77Code to encode the value from the encodeIndex
inline LZ77Code getCode(const std::vector<uint8_t>& vSignal, size_t encodeIndex, size_t N, size_t n1)
{
	uint8_t startSymbol = vSignal[encodeIndex];
	
	std::vector<LZ77Code> vCode;

	// Bound the windows
	uint8_t lowerOffset = n1;
	uint8_t upperOffset = N - n1;

	uint8_t currentOffset = 1;

	while (currentOffset < lowerOffset)
	{
		// Search between lowerIndex and upperIndex
		LZ77Code possibleVCode = searchPartial(vSignal, encodeIndex, currentOffset, lowerOffset, upperOffset);
		vCode.push_back(possibleVCode);
		if (possibleVCode.nLength > 0)
		{
			currentOffset = possibleVCode.nIdx + 1;
		}
		else 
		{
			break;
		}
		
	}

	// If none is found
	LZ77Code longestSequenceCode = vCode[0];
	
	uint8_t bestLength = longestSequenceCode.nLength;

	for (int index = 1; index < vCode.size(); ++index)
	{
		LZ77Code currentVCode = vCode[index];
		if (currentVCode.nLength > bestLength)
		{
			bestLength = currentVCode.nLength;
			longestSequenceCode = currentVCode;
		}
	}

	return longestSequenceCode;
}

// Return the first possible LZ77Code for the value at encodeIndex starting from currentIndex and backing until lowerIndex (limit of sliding windows)
inline LZ77Code searchPartial(const std::vector<uint8_t>& vSignal, size_t encodeIndex, uint8_t currentOffset, uint8_t lowerOffset, uint8_t upperOffset)
{
	//****** Find first occurence of same symbol *******
	uint8_t symbolToFind = vSignal[encodeIndex];
	uint8_t lookingOffset = currentOffset;
	uint8_t symbolFoundOffset;

	bool wasFound = false;

	while (lookingOffset < lowerOffset)
	{
		// Get symbol at this offset
		uint8_t foundSymbol;

		

		// Simulate the initialisaiton of the dictionnary with 0
		if (lookingOffset > encodeIndex)
		{
			foundSymbol = 0;
		}
		else {
			size_t indexToLook = encodeIndex - lookingOffset;
			foundSymbol = vSignal[indexToLook];
		}

		if (foundSymbol == symbolToFind)
		{
			wasFound = true;
			symbolFoundOffset = lookingOffset;
			break;
		}
		++lookingOffset;
	}


	//******* Find the length *********

	// If not found, we return an index of 0, lenght of 0 and tell that the next symbol that was not encode is the one we didn't find
	if (!wasFound)
	{
		LZ77Code notFindCode = {
			0,
			0,
			vSignal[encodeIndex]
		};

		return notFindCode;
	}


	long lookingIndex = (encodeIndex) - symbolFoundOffset;
	size_t upperIndex = encodeIndex + upperOffset;
	size_t currentLength = 0;

	bool smaller = lookingIndex < upperIndex;

	// sorry...
	while ((lookingIndex<0) || (lookingIndex < upperIndex && lookingIndex < vSignal.size()))
	{
		uint8_t symbolInDictionary = 0;
		if (lookingIndex >= 0)
		{
			symbolInDictionary = vSignal[lookingIndex];
		}

		uint8_t nextSymbolToEncode = vSignal[encodeIndex + currentLength];
		if (symbolInDictionary == nextSymbolToEncode)
		{
			++currentLength;
			++lookingIndex;
		}
		else 
		{
			break;
		}
	}

	// Get next symbol (the one not encoded)
	uint8_t nextSymbol;
	size_t nextSymbokIndex = encodeIndex + currentLength;
	if (nextSymbokIndex > vSignal.size())
	{
		nextSymbol = NULL;
	}
	else
	{
		nextSymbol = vSignal[nextSymbokIndex];
	}


	LZ77Code findCode = {
		symbolFoundOffset,
		currentLength,
		nextSymbol
	};

	return findCode;
}



inline std::vector<uint8_t> lz77_decode(const std::vector<LZ77Code>& vCode, size_t N, size_t n1) {
    CV_Assert(N>0 && n1>0 && N>n1 && n1<UCHAR_MAX && !vCode.empty());
    std::vector<uint8_t> vSignal;
   
	// For each code
	for (int codeIndex = 0; codeIndex < vCode.size(); ++codeIndex)
	{
		LZ77Code currentCode = vCode[codeIndex];
		// Get target symbol
		int index = vSignal.size() - currentCode.nIdx;

		uint8_t counterOfSymbolToAppend = currentCode.nLength;
		while (counterOfSymbolToAppend > 0)
		{
			uint8_t symbolAtOffset;
			if (index < 0)
			{
				symbolAtOffset = 0;
			}
			else 
			{
				symbolAtOffset = vSignal[index];
			}
			vSignal.push_back(symbolAtOffset);

			++index;
			--counterOfSymbolToAppend;
		}
		vSignal.push_back(currentCode.nNextSymbol);
	}


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
