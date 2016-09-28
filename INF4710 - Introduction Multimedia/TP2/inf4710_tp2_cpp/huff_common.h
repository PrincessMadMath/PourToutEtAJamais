
#pragma once

#include "tp2.h"

using HuffCode = std::vector<bool>;
template<typename T>
using HuffMap = std::map<T,HuffCode>;
template<typename T>
using HuffReverseMap = std::map<HuffCode,T>;

template<typename T>
struct HuffOutput {
    HuffCode string;
    HuffMap<T> map;
};

class INode {
public:
    const int f;
    virtual ~INode() {}
protected:
    INode(int f_init) : f(f_init) {}
};

class InternalNode : public INode {
public:
    INode *const left;
    INode *const right;
    InternalNode(INode* c0, INode* c1) : INode(c0->f + c1->f), left(c0), right(c1) {}
    ~InternalNode() {
        delete left;
        delete right;
    }
};

template<typename T>
class LeafNode : public INode {
public:
    const T c;
    LeafNode(int f_init, T c_init) : INode(f_init), c(c_init) {}
};

struct NodeCmp {
    bool operator()(const INode* lhs, const INode* rhs) const { return lhs->f > rhs->f; }
};

template<typename T,size_t NF>
inline INode* BuildTree(const std::array<int,NF>& frequencies) {
    std::priority_queue<INode*,std::vector<INode*>,NodeCmp> trees;
    for(size_t i=0; i<frequencies.size(); ++i) {
        if(frequencies[i]!=0)
            trees.push(new LeafNode<T>(frequencies[i],(T)i));
    }
    while(trees.size() > 1) {
        INode* childR = trees.top();
        trees.pop();
        INode* childL = trees.top();
        trees.pop();
        INode* parent = new InternalNode(childR, childL);
        trees.push(parent);
    }
    return trees.top();
}

template<typename T>
inline void GenerateCodes(const INode* node, const HuffCode& prefix, std::map<T,HuffCode>& outCodes) {
    if(const LeafNode<T>* lf = dynamic_cast<const LeafNode<T>*>(node))
        outCodes[lf->c] = prefix;
    else if(const InternalNode* in = dynamic_cast<const InternalNode*>(node)) {
        HuffCode leftPrefix = prefix;
        leftPrefix.push_back(false);
        GenerateCodes(in->left, leftPrefix, outCodes);
        HuffCode rightPrefix = prefix;
        rightPrefix.push_back(true);
        GenerateCodes(in->right, rightPrefix, outCodes);
    }
}
