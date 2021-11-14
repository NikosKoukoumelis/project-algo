#ifndef __HASHTABLE_H__
#define __HASHTABLE_H__

#include "TableItemTypes.h"

class HashTable
{
private:
    // h function parameters
    std::vector<float> *v;
    float *t;
    unsigned w;

    // hash function g parameters
    int *r;
    unsigned M;
    unsigned _k;

    // hash table parameters
    unsigned _numOfBuckets;
    unsigned _dimension;
    std::vector<HashTableItem> *table;

    long h(const std::vector<float> &input, const unsigned i);
    long g(const std::vector<float> &input);

public:
    HashTable(unsigned numOfBuckets, unsigned k, unsigned dimension);
    ~HashTable();

    void push(std::vector<float> input);
    std::vector<HashTableItem>::iterator getBucketStart(const std::vector<float> &input);
    std::vector<HashTableItem>::iterator getBucketEnd(const std::vector<float> &input);

    long getHashId(const std::vector<float> &input);
};

#endif
