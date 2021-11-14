#include <vector>
#include <map>
#include <cmath>
#include <random>
#include <chrono>
#include <iostream>
#include <algorithm>
#include <sys/time.h>
#include "../headers/Hypercube.h"

Hypercube::Hypercube(unsigned k, unsigned dimension)                //hypercube constructor
{
    _k = k;
    _dimension = dimension;                 //dimention is the dimention of the input vectors

    v = new std::vector<float>[_k];
    t = new float[_k];
    w = 10;

    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine generator(seed);
    std::normal_distribution<float> distribution(0.0, 1.0);

    std::random_device rd;  // Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd()); // Standard mersenne_twister_engine seeded with rd()
    std::uniform_real_distribution<float> dis(0.0, (float)w);

    for (size_t i = 0; i < _k; i++)
    {
        for (size_t j = 0; j < _dimension; j++)
        {
            float randomCoordinate;
            randomCoordinate = distribution(generator);
            v[i].push_back(randomCoordinate);
        }
        t[i] = dis(gen);
    }

    fmap = new std::map<long, char>[_k];
    table = new std::vector<HypercubeItem>[(int)pow((float)2, (float)_k)];  //table has size 2^k
}

Hypercube::~Hypercube()//destructor
{
    delete[] v;
    delete[] t;
    delete[] fmap;
    delete[] table;
}

void Hypercube::push(std::vector<float> input)//push into the hypercube
{
    HypercubeItem item;

    item.itemId = *(input.begin());
    input.erase(input.begin());

    long index = 0;
    long w = 1;
    char currbit;
    for (size_t i = 0; i < _k; i++)                 //find index to the table using f
    {
        currbit = f(input, i);
        index += w * currbit;
        w *= 2;
    }

    item.v = input;
    table[index].push_back(item);
}

std::vector<HypercubeItem>::iterator Hypercube::getBucketStart(const std::vector<float> &input)
{
    long index = 0;
    long w = 1;
    char currbit;
    for (size_t i = 0; i < _k; i++)
    {
        currbit = f(input, i);
        index += w * currbit;
        w *= 2;
    }

    return table[index].begin();
}
long Hypercube::getBucketNo(const std::vector<float> &input)
{
    long index = 0;
    long w = 1;
    char currbit;
    for (size_t i = 0; i < _k; i++)
    {
        currbit = f(input, i);
        index += w * currbit;
        w *= 2;
    }

    return index;
}

std::vector<HypercubeItem>::iterator Hypercube::getBucketEnd(const std::vector<float> &input)
{
    long index = 0;
    long w = 1;
    char currbit;
    for (size_t i = 0; i < _k; i++)
    {
        currbit = f(input, i);
        index += w * currbit;
        w *= 2;
    }

    return table[index].end();
}

std::vector<HypercubeItem>::iterator Hypercube::getBucketEnd(const int index)
{

    return table[index].end();
}

std::vector<HypercubeItem>::iterator Hypercube::getBucketStart(const int index)
{

    return table[index].begin();
}

unsigned Hypercube::getNumOfBuckets()
{
    return (unsigned)pow((float)2, (float)_k);
}

long Hypercube::h(const std::vector<float> &input, const unsigned i)//h function just like the slides
{
    float dotProduct = 0.0;

    for (size_t j = 0; j < input.size(); j++)
    {
        
        
            dotProduct += input.at(j) * v[i].at(j);
        
        
    }

    float res;

    res = (dotProduct + t[i]) / (float)w;

    return (long)floor(res);
}

char Hypercube::f(const std::vector<float> &input, const unsigned i)//f function just like the slides
{
    long key = h(input, i);
    try
    {

        return fmap[i].at(key);
    }
    catch (const std::out_of_range &e)
    {
        char n = rand() % 2;
        fmap[i][key] = n;

        return n;
    }
}
