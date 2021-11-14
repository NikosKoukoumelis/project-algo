#ifndef __HYPERCUBE_H__
#define __HYPERCUBE_H__

#include "TableItemTypes.h"
#include <vector>
#include <map>

class Hypercube
{
private:
    // h function parameters
    std::vector<float> *v;
    float *t;
    unsigned w;

    unsigned _k;
    std::map<long, char> *fmap;                     //to implement f function use fmap

    // hypercube parameters
    unsigned _dimension;
    std::vector<HypercubeItem> *table;

    long h(const std::vector<float> &input, const unsigned i);
    char f(const std::vector<float> &input, const unsigned i);

public:
    Hypercube(unsigned k, unsigned dimension);
    ~Hypercube();

    void push(std::vector<float> input);
    std::vector<HypercubeItem>::iterator getBucketStart(const std::vector<float> &input);
    std::vector<HypercubeItem>::iterator getBucketEnd(const std::vector<float> &input);
    std::vector<HypercubeItem>::iterator getBucketStart(const int index);
    std::vector<HypercubeItem>::iterator getBucketEnd(const int index);
    unsigned getNumOfBuckets();
    long getBucketNo(const std::vector<float> &input);
};

#endif
