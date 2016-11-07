
#pragma once

#include "huff_common.h"

template<size_t n=64, typename T=short>
inline HuffOutput<T> huff(const std::vector<std::array<T,n>>& vv) {
    static_assert(sizeof(T)<4,"Do you really have 16GB to spend?"); // replace by unordered map for larger symbol sets
    std::array<int,((size_t)1)<<sizeof(T)*8> anSymbolFrequencies = {};
    for(size_t i=0; i<vv.size(); ++i)
        for(size_t j=0; j<vv[i].size(); ++j)
            ++anSymbolFrequencies[(typename std::make_unsigned<T>::type)(vv[i][j])];
    Inode* pRoot = BuildTree<T>(anSymbolFrequencies);
    HuffOutput<T> oOutput;
    GenerateCodes<T>(pRoot,HuffCode(),oOutput.map);
    delete pRoot;
    for(size_t i=0; i<vv.size(); ++i) {
        for(size_t j=0; j<vv[i].size(); ++j) {
            const HuffCode& vbCurrString = oOutput.map.at(vv[i][j]);
            oOutput.string.insert(oOutput.string.end(),vbCurrString.begin(),vbCurrString.end());
        }
    }
    return oOutput;
}
