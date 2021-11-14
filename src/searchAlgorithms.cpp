#include <vector>
#include <algorithm>
#include <limits>
#include <iostream>
#include "../headers/searchAlgorithms.h"
#include "../headers/util.h"

HashTableItem *lsh(std::vector<float> input, HashTable **tables, unsigned L, unsigned N, float *distances, float (*metric)(const std::vector<float> a, const std::vector<float> b))
{
    HashTableItem *NearestNeighbors = new HashTableItem[N];

    for (size_t i = 0; i < N; i++)
    {
        distances[i] = std::numeric_limits<float>::max();
    }

    int countItemsSameId = 0;
    for (size_t i = 0; i < L; i++)
    {

        long hashId = tables[i]->getHashId(input);
        std::vector<HashTableItem>::iterator it, bucketStart = tables[i]->getBucketStart(input), bucketEnd = tables[i]->getBucketEnd(input);
        for (it = bucketStart; it != bucketEnd; it++)
        {
            if (it->hashId == hashId)
            {
                countItemsSameId++;
                float dist = metric(it->v, input);

                for (size_t j = 0; j < N; j++)
                {
                    if (dist < distances[j])
                    {
                        for (size_t jj = N - 1; jj > j; jj--)
                        {
                            distances[jj] = distances[jj - 1];
                            NearestNeighbors[jj] = NearestNeighbors[jj - 1];
                        }
                        distances[j] = dist;
                        NearestNeighbors[j] = *it;
                        break;
                    }
                    else if (dist == distances[j] && it->itemId == NearestNeighbors[j].itemId)
                    {
                        break;
                    }
                }
            }
        }
    }
    if((unsigned int)countItemsSameId <= 3*N)         //if it found too little items with same id, redo the lsh withous the id trick
    {
        for (size_t i = 0; i < L; i++)
        {

            std::vector<HashTableItem>::iterator it, bucketStart = tables[i]->getBucketStart(input), bucketEnd = tables[i]->getBucketEnd(input);
            for (it = bucketStart; it != bucketEnd; it++)
            {
                float dist = metric(it->v, input);

                for (size_t j = 0; j < N; j++)
                {
                    if (dist < distances[j])
                    {
                        for (size_t jj = N - 1; jj > j; jj--)
                        {
                            distances[jj] = distances[jj - 1];
                            NearestNeighbors[jj] = NearestNeighbors[jj - 1];
                        }
                        distances[j] = dist;
                        NearestNeighbors[j] = *it;
                        break;
                    }
                    else if (dist == distances[j] && it->itemId == NearestNeighbors[j].itemId)
                    {
                        break;
                    }
                }
            }
        }
    }

    return NearestNeighbors;
}

std::vector<HashTableItem> lsh_rangeSearch(std::vector<float> input, HashTable **tables, unsigned L, float R, bool clusterUsage, float (*metric)(const std::vector<float> a, const std::vector<float> b))
{
    std::vector<HashTableItem> nearestNeighbors;

    if (clusterUsage) //when algorithm is used for clustering, DON'T use the ID's trick!
    {
        for (size_t i = 0; i < L; i++)
        {

            std::vector<HashTableItem>::iterator it, bucketStart = tables[i]->getBucketStart(input), bucketEnd = tables[i]->getBucketEnd(input);
            for (it = bucketStart; it != bucketEnd; it++)
            {
                float dist = metric(it->v, input);

                if (dist < R)
                {
                    nearestNeighbors.push_back(*it);
                }
            }
        }
        return nearestNeighbors;
    }

    for (size_t i = 0; i < L; i++)
    {

        long hashId = tables[i]->getHashId(input);
        std::vector<HashTableItem>::iterator it, bucketStart = tables[i]->getBucketStart(input), bucketEnd = tables[i]->getBucketEnd(input);
        for (it = bucketStart; it != bucketEnd; it++)
        {
            if (it->hashId == hashId)
            {
                float dist = metric(it->v, input);

                if (dist < R)
                {
                    nearestNeighbors.push_back(*it);
                }
            }
        }
    }

    return nearestNeighbors;
}

HypercubeItem *hypercube_search(std::vector<float> input, Hypercube &hypercube, unsigned M, unsigned probes, unsigned N, float *distances, float (*metric)(const std::vector<float> a, const std::vector<float> b))
{
    HypercubeItem *NearestNeighbors = new HypercubeItem[N];

    for (size_t i = 0; i < N; i++)
    {
        distances[i] = std::numeric_limits<float>::max();
    }

    unsigned vectorsTested = 0, verticesVisited = 0;
    unsigned inputHam = hypercube.getBucketNo(input);

    std::vector<int> unvisitedVertices;

    for (size_t i = 0; i < hypercube.getNumOfBuckets(); i++)
    {
        unvisitedVertices.push_back(i);
    }
    while (vectorsTested <= M)
    {

        int minHammingVertex = std::numeric_limits<int>::max(), minVertex;
        std::vector<int>::iterator minit, vectrorStart = unvisitedVertices.begin(), vectrorEnd = unvisitedVertices.end();
        for (minit = vectrorStart; minit != vectrorEnd; minit++)
        {

            if (hamming(*minit, inputHam) < minHammingVertex)
            {
                minVertex = *minit;
                minHammingVertex = hamming(*minit, inputHam);
            }
        }
        std::vector<int>::iterator p = find(unvisitedVertices.begin(), unvisitedVertices.end(), minVertex);
        unvisitedVertices.erase(p);

        std::vector<HypercubeItem>::iterator it, bucketStart = hypercube.getBucketStart(minVertex), bucketEnd = hypercube.getBucketEnd(minVertex);
        for (it = bucketStart; it != bucketEnd && vectorsTested <= M; it++)
        {
            float dist = metric(it->v, input);
            for (size_t j = 0; j < N; j++)
            {
                if (dist < distances[j])
                {
                    for (size_t jj = N - 1; jj > j; jj--)
                    {
                        distances[jj] = distances[jj - 1];
                        NearestNeighbors[jj] = NearestNeighbors[jj - 1];
                    }
                    distances[j] = dist;
                    NearestNeighbors[j] = *it;
                    break;
                }
                else if (dist == distances[j] && it->itemId == NearestNeighbors[j].itemId)
                {
                    break;
                }
            }
            vectorsTested++;
        }
        verticesVisited++;
        if (verticesVisited >= probes)
        {
            break;
        }
    }

    return NearestNeighbors;
}

std::vector<HypercubeItem> hypercube_rangesearch(std::vector<float> input, Hypercube &hypercube, unsigned M, unsigned probes, unsigned R, float (*metric)(const std::vector<float> a, const std::vector<float> b))
{
    std::vector<HypercubeItem> res;

    unsigned inputHam = hypercube.getBucketNo(input);
    unsigned vectorsTested = 0, verticesVisited = 0;


    std::vector<int> unvisitedVertices;
    for (unsigned int i = 0; i < hypercube.getNumOfBuckets(); i++)
    {
        unvisitedVertices.push_back(i);
    }

    while (vectorsTested <= M)
    {

        int minHammingVertex = std::numeric_limits<int>::max(), minVertex;
        std::vector<int>::iterator minit, vectrorStart = unvisitedVertices.begin(), vectrorEnd = unvisitedVertices.end();
        for (minit = vectrorStart; minit != vectrorEnd; minit++)
        {

            if (hamming(*minit, inputHam) < minHammingVertex)
            {
                minVertex = *minit;
                minHammingVertex = hamming(*minit, inputHam);
            }
        }

        std::vector<int>::iterator p = find(unvisitedVertices.begin(), unvisitedVertices.end(), minVertex);
        unvisitedVertices.erase(p);

        std::vector<HypercubeItem>::iterator it, bucketStart = hypercube.getBucketStart(minVertex), bucketEnd = hypercube.getBucketEnd(minVertex);
        //need flag, got into the for
        for (it = bucketStart; it != bucketEnd && vectorsTested < M; it++) //sometimes it doesnt get into for cause this "bucket" is empty.
        {
            float dist = metric(it->v, input);

            if (dist < R)
            {
                res.push_back(*it);
            }
            vectorsTested++;
        }
        verticesVisited++;
        if (verticesVisited >= probes)
        {
            break;
        }
    }

    return res;
}

HypercubeItem *true_distance_NN(std::vector<float> input, HypercubeItem *vTable, unsigned size, unsigned N, float *distances, float (*metric)(const std::vector<float> a, const std::vector<float> b))
{
    HypercubeItem *NearestNeighbors = new HypercubeItem[N];

    for (size_t i = 0; i < N; i++)
    {
        distances[i] = std::numeric_limits<float>::max();
    }

    for (size_t i = 0; i < size; i++)
    {
        HypercubeItem currItem = vTable[i];
        float dist = metric(currItem.v, input);

        for (size_t j = 0; j < N; j++)
        {
            if (dist < distances[j])
            {
                for (size_t jj = N - 1; jj > j; jj--)
                {
                    distances[jj] = distances[jj - 1];
                    NearestNeighbors[jj] = NearestNeighbors[jj - 1];
                }
                distances[j] = dist;
                NearestNeighbors[j] = currItem;
                break;
            }
            else if (dist == distances[j] && currItem.itemId == NearestNeighbors[j].itemId)
            {
                break;
            }
        }
    }
    return NearestNeighbors;
}
 