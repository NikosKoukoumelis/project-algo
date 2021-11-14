#ifndef __CLUSTERINGALGOS_H__
#define __CLUSTERINGALGOS_H__

#include <vector>
#include "Cluster.h"

void lloyds(Cluster *clusters, unsigned numOfClusters, HypercubeItem *inputVectors, int inputSize);
void reverse_approach_lsh(Cluster *clusters, unsigned numOfClusters, std::vector<std::vector<float>> input, int L, int k, int dimention, float (*metric)(const std::vector<float> a, const std::vector<float> b)); //here input is as given with item_id inside
void reverse_approach_hyper(Cluster *clusters, unsigned numOfClusters, std::vector<std::vector<float>> input, int M, int probes, int k, float (*metric)(const std::vector<float> a, const std::vector<float> b));//here input is as given with item_id inside
#endif
