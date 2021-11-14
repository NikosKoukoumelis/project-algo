#ifndef __CLUSTER_H__
#define __CLUSTER_H__

#include <vector>

struct Cluster                                          //Cluster, contains a centroid and items in the cluster
{
    std::vector<HypercubeItem> clusterVectors;         //HypercubeItem contains, a vector of an item and its item_id
    std::vector<float> centroid;
};

#endif
