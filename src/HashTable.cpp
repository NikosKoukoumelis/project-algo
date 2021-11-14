#include <vector>
#include <cmath>
#include <random>
#include <chrono>
#include <iostream>
#include <algorithm>
#include "../headers/HashTable.h"
#include "../headers/util.h"

HashTable::HashTable(unsigned numOfBuckets, unsigned k, unsigned dimension)     //Hash table constructor
{
    _numOfBuckets = numOfBuckets;                                               //Hash table implementantion just like pdf slides
    _k = k;
    _dimension = dimension;

    v = new std::vector<float>[_k];
    t = new float[_k];
    r = new int[_k];
    w = 10;                               //w is se to 50
    M = 4294967291;

    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine generator(seed);
    std::normal_distribution<float> distribution(0.0, 1.0);

    std::random_device rd;  // Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd()); // Standard mersenne_twister_engine seeded with rd()
    std::uniform_real_distribution<float> dis(0.0, (float)w);
    std::uniform_int_distribution<int> dis2(-100, 100);

    for (size_t i = 0; i < _k; i++)
    {
        for (size_t j = 0; j < _dimension; j++)
        {
            float randomCoordinate;
            randomCoordinate = distribution(generator);
            v[i].push_back(randomCoordinate);
        }
        t[i] = dis(gen);
        r[i] = dis2(gen);
    }

    table = new std::vector<HashTableItem>[_numOfBuckets];
}

HashTable::~HashTable(){//destructor
    delete[] v;
    delete[] t;
    delete[] r;
    delete[] table;
}

void HashTable::push(std::vector<float> input)//push into hash table an item
{
    HashTableItem item;

    item.itemId = *(input.begin());
    input.erase(input.begin());
    long hashId = g(input);

    long index = mod(hashId, _numOfBuckets);

    item.hashId = hashId;
    item.v = input;
    table[index].push_back(item);
}


std::vector<HashTableItem>::iterator HashTable::getBucketStart(const std::vector<float> &input)
{
    long index = mod(g(input), _numOfBuckets);

    return table[index].begin();
}

std::vector<HashTableItem>::iterator HashTable::getBucketEnd(const std::vector<float> &input)
{
    long index = mod(g(input), _numOfBuckets);

    return table[index].end();
}

long HashTable::getHashId(const std::vector<float> &input)
{
    return g(input);
}

long HashTable::h(const std::vector<float> &input, const unsigned i)//h funtion implementation just like pdf slides
{
    float dotProduct = 0.0;

    for (size_t j = 0; j < input.size(); j++)
    {
        try
        {
            dotProduct += input.at(j) * v[i].at(j);
        }
        catch (const std::exception &e)
        {
            std::cerr << e.what() << input.size() << " at call in h\n";
        }
    }

    float res;

    res = (dotProduct + t[i]) / (float)w;

    return (long)floor(res);
}

long HashTable::g(const std::vector<float> &input)//g implementation just like pdf slides
{
    long result = 0;

    for (size_t i = 0; i < _k; i++)
    {
        long a, b, c;

        a = mod(r[i], M);
        b = mod(h(input, i), M);
        c = mod(a * b, M);
        result = mod(result + c, M);
    }
    return result;
}
