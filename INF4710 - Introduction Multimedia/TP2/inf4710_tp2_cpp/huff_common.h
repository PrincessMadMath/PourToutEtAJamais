
#pragma once

#include "tp2.h"
#include <unordered_map>

using HuffCode = std::vector<bool>;
template<typename T>
using HuffMap = std::map<T,HuffCode>;
template<typename T>
using HuffReverseMap = std::unordered_map<HuffCode,T>;

template<typename T>
struct HuffOutput {
    HuffCode string;
    HuffMap<T> map;
};

class Inode {
public:
    const int f;
    virtual ~Inode() {}
protected:
    Inode(int f_init) : f(f_init) {}
};

class Internalnode : public Inode {
public:
    Inode *const left;
    Inode *const right;
    Internalnode(Inode* c0, Inode* c1) : Inode(c0->f + c1->f), left(c0), right(c1) {}
    ~Internalnode() {
        delete left;
        delete right;
    }
};

template<typename T>
class Leafnode : public Inode {
public:
    const T c;
    Leafnode(int f_init, T c_init) : Inode(f_init), c(c_init) {}
};

struct nodeCmp {
    bool operator()(const Inode* lhs, const Inode* rhs) const { return lhs->f > rhs->f; }
};

template<typename T,size_t nF>
inline Inode* BuildTree(const std::array<int,nF>& frequencies) {
    std::priority_queue<Inode*,std::vector<Inode*>,nodeCmp> trees;
    for(size_t i=0; i<frequencies.size(); ++i) {
        if(frequencies[i]!=0)
            trees.push(new Leafnode<T>(frequencies[i],(T)i));
    }
    while(trees.size() > 1) {
        Inode* childR = trees.top();
        trees.pop();
        Inode* childL = trees.top();
        trees.pop();
        Inode* parent = new Internalnode(childR, childL);
        trees.push(parent);
    }
    return trees.top();
}

template<typename T>
inline void GenerateCodes(const Inode* node, const HuffCode& prefix, std::map<T,HuffCode>& outCodes) {
    if(const Leafnode<T>* lf = dynamic_cast<const Leafnode<T>*>(node))
        outCodes[lf->c] = prefix;
    else if(const Internalnode* in = dynamic_cast<const Internalnode*>(node)) {
        HuffCode leftPrefix = prefix;
        leftPrefix.push_back(false);
        GenerateCodes(in->left, leftPrefix, outCodes);
        HuffCode rightPrefix = prefix;
        rightPrefix.push_back(true);
        GenerateCodes(in->right, rightPrefix, outCodes);
    }
}
