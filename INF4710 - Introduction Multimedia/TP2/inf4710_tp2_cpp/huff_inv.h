
#pragma once

#include "huff_common.h"
#include <algorithm>    // std::min

template<size_t n=64, typename T=short>
inline std::vector<std::array<T,n>> huff_inv(const HuffOutput<T>& oInput) {
    CV_Assert(!oInput.map.empty() && !oInput.string.empty());
    std::vector<std::array<T,n>> vvOutput;

	HuffReverseMap<T> reverseMap;

	int minLength = INT32_MAX;

	for (const auto& sm_pair : oInput.map)
	{
		minLength = (minLength< sm_pair.second.size()) ? minLength : sm_pair.second.size();
		reverseMap.insert(std::pair<HuffCode, T>(sm_pair.second, sm_pair.first));
	}

	int elementCounter = 0;
	std::array<T, n> block;
	HuffCode temp;


	for (int index = 0; index < oInput.string.size(); ++index)
	{
		temp.push_back(oInput.string[index]);
		
		if (temp.size() >= minLength)
		{
			const auto& iterator = reverseMap.find(temp);
			if (iterator != reverseMap.end())
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
	}
   
    return vvOutput;
}
