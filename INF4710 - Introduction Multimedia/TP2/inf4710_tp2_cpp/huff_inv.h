
#pragma once

#include "huff_common.h"

template<size_t n=64, typename T=short>
inline std::vector<std::array<T,n>> huff_inv(const HuffOutput<T>& oInput) {
    CV_Assert(!oInput.map.empty() && !oInput.string.empty());
    std::vector<std::array<T,n>> vvOutput;

	HuffReverseMap<T> reverseMap;

	for (const auto& sm_pair : oInput.map)
	{
		reverseMap.insert(std::pair<HuffCode, T>(sm_pair.second, sm_pair.first));
	}

	int elementCounter = 0;
	std::array<T, n> block;
	HuffCode temp;


	for (int index = 0; index < oInput.string.size(); ++index)
	{
		temp.push_back(oInput.string[index]);
		if (reverseMap.find(temp) != reverseMap.end())
		{
			T value = reverseMap.at(temp);

			block[elementCounter] = value;

			++elementCounter;

			if (elementCounter == n)
			{
				vvOutput.push_back(block);
				elementCounter = 0;
			}

			temp.clear();
		}
	}
   
    return vvOutput;
}
