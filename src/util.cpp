#include <vector>
#include <cmath>
#include <fstream>
#include <string>
#include <iostream>
#include <random>
#include "../headers/HashTable.h"
#include "../headers/searchAlgorithms.h"
#include "../headers/util.h"


long mod(const long a, const long b)//modulo functions
{
    long c = a % b;
    return (c < 0) ? c + b : c;
}

float euclidean(const std::vector<float> a, const std::vector<float> b)//euclidean distance between tow vecrtors
{
    float result = 0.0;
    if (a.size() != b.size())
    {
        // error case
        std::cerr << "mismatched dimensions" << std::endl;
        return -1.0;
    }

    for (size_t i = 0; i < a.size(); i++)
    {
        result += (a.at(i) - b.at(i)) * (a.at(i) - b.at(i));
    }

    return sqrt(result);
}

int hamming(const int a, const int b)                                   //hamming distance between 2 integers
{
    int res = 0, temp = a ^ b;

    while (temp)
    {
        res += temp & 1;
        temp >>= 1;
    }
    return res;
}

std::vector<std::vector<float>> inputFileParser(std::string filename)   //parses a file with vectors and returns the vectors
{
    std::vector<std::vector<float>> vectorContainer;
    std::vector<float> currentVector;

    std::ifstream infile;
    infile.open(filename);

    std::string currentLine;
    while (getline(infile, currentLine))
    {

        currentVector.clear();
        currentLine.pop_back();
        currentLine.pop_back();
        std::string delimiter = " ";
        size_t pos = 0;
        std::string token;
        while ((pos = currentLine.find(delimiter)) != std::string::npos)
        {
            token = currentLine.substr(0, pos);
            currentVector.push_back(std::stof(token));
            currentLine.erase(0, pos + delimiter.length());
        }
        token = currentLine;
        currentVector.push_back(std::stof(token));

        vectorContainer.push_back(currentVector);
    }

    infile.close();
    return vectorContainer;
}

std::vector<std::vector<float>> kMeansInit(unsigned numOfClusters, std::vector<std::vector<float>> input, float (*metric)(const std::vector<float> a, const std::vector<float> b))//kmeans initialization
{
    std::vector<std::vector<float>> centroids;
    unsigned inputSize = input.size();

    std::random_device rd;  //seed for random number engine
    std::mt19937 gen(rd()); 
    std::uniform_int_distribution<int> dis2(0, inputSize - 1);

    centroids.push_back(input.at(dis2(gen))); //first pick a random centroid with uniform distribution
    double maxDi = 10000000.0;//to normilize the results
    for (unsigned int t = 1; t < numOfClusters; t++)
    {
        double *P = new double[inputSize - t + 1]; //P array as asked
        P[0] = 0;
        int *canditateCentroidIndex = new int[inputSize - t + 1];//canditateCentroidIndex keeps the vectors that are not already centroids but are candidates
        canditateCentroidIndex[0] = -1; //neverused
        

        int index = 1;
        for (unsigned int i = 0; i < inputSize; i++)
        {
            //find Di for every input
            double Di;
            double minDi = std::numeric_limits<double>::max();
            bool inputInCentroids = false;

            for (std::vector<std::vector<float>>::iterator it = centroids.begin(); it != centroids.end(); it++)
            {

                if (*it == input.at(i))
                {
                    inputInCentroids = true;
                    break;
                }
            }
            if (inputInCentroids)
                continue;

            for (std::vector<std::vector<float>>::iterator it = centroids.begin(); it != centroids.end(); it++)
            {
                Di = euclidean(*it, input.at(i));
                if (Di < minDi)
                {
                    minDi = Di;
                }
            }
            Di = minDi/maxDi;
            P[index] = P[index - 1] + Di * Di;
            canditateCentroidIndex[index] = i;
            index++;
        }
        //picking random x from [0, P[n]) with uniform distribution
        std::uniform_real_distribution<float> dis(0.0, P[inputSize - t]);
        float x = dis(gen);
        
        //using binary search to find the suitable r
        int low = 1, high = inputSize - t, r = 0;
        while (low <= high)
        {
            r = (low + high) / 2;
            if (P[r - 1] < x && x <= P[r])
            {
                break;
            }
            else if (P[r - 1] >= x)
            {
                high = r - 1;
            }
            else
            {
                low = r + 1;
            }
        }
        //push new centroid into centroids
        centroids.push_back(input.at(canditateCentroidIndex[r]));
        delete[] P;
        delete[] canditateCentroidIndex;
    }
    //return the centroids found using kmeans
    return centroids;
}




void silhouette(Cluster *clusters, int numOfClusters, HypercubeItem *inputVectors, int inputSize, int dimentionOfVectors, std::ofstream &outputfile){
    
    float *A = new float[inputSize];                 //A[i] as at a slides
    float *B = new float[inputSize];                 //B[i] as at slides
    float *SofClusters = new float[numOfClusters];   //S for every cluster(not for every item), average S from items in clusterS


    for(int i = 0; i < inputSize; i++)
    {

        int clusterIndex;
        for(int j = 0; j < numOfClusters; j++)
        {

            std::vector<HypercubeItem>::iterator it = find(clusters[j].clusterVectors.begin(), clusters[j].clusterVectors.end(), inputVectors[i]);
            if(it != clusters[j].clusterVectors.end()){
                clusterIndex = j;
                break;
            }
        }

    
        float sum = 0;
        for(std::vector<HypercubeItem>::iterator it = clusters[clusterIndex].clusterVectors.begin(); it != clusters[clusterIndex].clusterVectors.end(); it++)
        {
            sum += euclidean(inputVectors[i].v, it->v);
        }
        A[i] = sum / (float)clusters[clusterIndex].clusterVectors.size();       //average distance from i item to all items that are in the same cluster as i

        

        float minDistFromOtherCluster = std::numeric_limits<float>::max();
        int secondClusterIndex;

        for(int j = 0; j < numOfClusters; j++)
        {   
            float distance = euclidean(clusters[j].centroid, inputVectors[i].v);
            if(distance < minDistFromOtherCluster)
            {
                minDistFromOtherCluster = distance;
                secondClusterIndex = j;
            }
        }
        
        sum = 0;
        for(std::vector<HypercubeItem>::iterator it = clusters[secondClusterIndex].clusterVectors.begin(); it != clusters[secondClusterIndex].clusterVectors.end(); it++)
        {
            sum += euclidean(inputVectors[i].v, it->v);
        }
        B[i] = sum / (float)clusters[secondClusterIndex].clusterVectors.size();  //average distance from i item to all items in the second best cluster for i


        if(A[i] < B[i])                                                         
        {
            SofClusters[clusterIndex] = SofClusters[clusterIndex] + (1.0 - A[i]/B[i]);
        }
        else if(A[i] > B[i])
        {
            SofClusters[clusterIndex] = SofClusters[clusterIndex] + (B[i]/A[i] - 1);
        }else
        {
            SofClusters[clusterIndex] = SofClusters[clusterIndex] + 0;
        }

    }


    //printing what the assigments say about sihlouette
    
    outputfile<<"Silhouette: [";
    float stotal = 0;
    for (int i = 0; i <numOfClusters ; i++)
    {
        SofClusters[i] /= (float)clusters[i].clusterVectors.size();
        outputfile<<SofClusters[i]<<", ";
        stotal += SofClusters[i];
    }
    outputfile<<stotal / (float) numOfClusters<<"]"<<std::endl;
    
}
