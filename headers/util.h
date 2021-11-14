#ifndef __UTIL_H__
#define __UTIL_H__

#include <vector>
#include "Cluster.h"

long mod(const long a, const long b);
float euclidean(const std::vector<float> a, const std::vector<float> b);
int hamming(const int a, const int b);
std::vector<std::vector<float>> inputFileParser(std::string filename);
std::vector<std::vector<float>> kMeansInit(unsigned numOfClusters, std::vector<std::vector<float>> input, float (*metric)(const std::vector<float> a, const std::vector<float> b));
void silhouette(Cluster *clusters, int numOfClusters, HypercubeItem *inputVectors, int inputSize, int dimentionOfVectors, std::ofstream &outputfile);
#endif
