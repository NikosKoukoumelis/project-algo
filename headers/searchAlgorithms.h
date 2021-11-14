#include <vector>
#include <algorithm>
#include <limits>
#include "HashTable.h"
#include "Hypercube.h"
//search algorithms
std::vector<HashTableItem> lsh_rangeSearch(std::vector<float> input, HashTable **tables, unsigned L, float R, bool clusterUsage, float (*metric)(const std::vector<float> a, const std::vector<float> b));
HashTableItem *lsh(std::vector<float> input, HashTable **tables, unsigned L, unsigned N, float *distances, float (*metric)(const std::vector<float> a, const std::vector<float> b));
HypercubeItem *hypercube_search(std::vector<float> input, Hypercube &hypercube, unsigned M, unsigned probes, unsigned N, float *distances, float (*metric)(const std::vector<float> a, const std::vector<float> b));
std::vector<HypercubeItem> hypercube_rangesearch(std::vector<float> input, Hypercube &hypercube, unsigned M, unsigned probes, unsigned R, float (*metric)(const std::vector<float> a, const std::vector<float> b));
HypercubeItem *true_distance_NN(std::vector<float> input, HypercubeItem *vTable, unsigned size, unsigned N, float *distances, float (*metric)(const std::vector<float> a, const std::vector<float> b));
