#include <iostream>
#include <cstring>
#include <fstream>
#include <random>
#include "../headers/Hypercube.h"
#include "../headers/searchAlgorithms.h"
#include "../headers/Cluster.h"
#include "../headers/util.h"
#include "../headers/clusteringAlgos.h"

int main(int argc, char *argv[])
{
    bool iFoundFlag = false, cFoundFlag = false, oFoundFlag = false, completeFlag = false;
    int numOfClusters = 10, L = 3, kLSH = 4, M = 10, kHYPER = 3, probes = 2;

    std::string flags[5] = {"-i", "-c", "-o", "-complete", "-m"};
    std::string inFile, outFile, configFile, method;
    int ClusterMethod;//0 for Classic, 1 for LSH, 2 for Hyper


    for (int i = 1; i < argc; i += 2)
    {
        int flagsIndex = -1;
       
        for (int j = 0; j < 7; j++)
        {
            if (!strcmp(argv[i], flags[j].c_str()))
            {
                flagsIndex = j;
                break;
            }
        }
        switch (flagsIndex)
        {
        case 0:
            inFile = std::string(argv[i + 1]);
            iFoundFlag = true;
            break;
        case 1:
            configFile = std::string(argv[i + 1]);
            cFoundFlag = true;
            break;
        case 2:
            outFile = std::string(argv[i + 1]);
            oFoundFlag = true;
            break;
        case 3:
            completeFlag = true;
            i--;
            break;
        case 4:
            method = std::string(argv[i + 1]);
            break;
        }
    }

    if (!oFoundFlag || !iFoundFlag || !cFoundFlag)
    {
        std::cout<<"input file or output file ot configuration file not givenm program finished!"<<std::endl;
        return 1; //error not given the files
    }

    
    std::ofstream outputfile(outFile);

    if(method == "Classic")
    {
        ClusterMethod = 0;
    }
    else if(method == "LSH")
    {
        ClusterMethod = 1;
    }
    else if(method == "Hypercube"){
        ClusterMethod = 2;
    }
    else{
        std::cout<<"Method: "<<method<<" doesn't exists, program finished!"<<std::endl;
        return 1;
    }

    std::vector<std::vector<float>> input = inputFileParser(inFile);
    int inputSize = input.size();
    HypercubeItem *inputVectors = new HypercubeItem[inputSize];
    std::vector<std::vector<float>> inVectors, centroids;
    int index = 0;
    for (std::vector<std::vector<float>>::iterator it = input.begin(); it != input.end(); it++)
    {
        HypercubeItem temp;
        std::vector<float> currInVector(*it);
        std::vector<float>::iterator vectorIt = currInVector.begin();
        temp.itemId = *vectorIt;
        currInVector.erase(vectorIt);
        temp.v = currInVector;
        inVectors.push_back(currInVector);
        inputVectors[index] = temp;
        index++;
    }

// Read configuration file


    std::ifstream ConfIfStream(configFile);

    std::string name;
    int content;
    while(ConfIfStream >> name >> content) {
        if(name == "number_of_clusters:") {
            numOfClusters = content;
        }
        else if(name == "number_of_vector_hash_tables:") {
            L = content;
        }
        else if(name == "number_of_vector_hash_functions:") {
            kLSH = content;
        }
        else if(name == "max_number_M_hypercube:") {
            M = content;
        }
        else if(name == "number_of_hypercube_dimensions:") {
            kHYPER = content;
        }
        else if(name == "number_of_probes:") {
            probes = content;
        }
        else {
            std::cout << "Error at configuration file, program finished!" << std::endl;
            return 1;
        }
    }


    //kmeans for centroid initialisation
    centroids = kMeansInit(numOfClusters, inVectors, euclidean);
    Cluster clusters[numOfClusters];
    for (int i = 0; i < numOfClusters; i++)
    {
        clusters[i].centroid = centroids.at(i);
    }


    clock_t start, stop, tclustering;
    start = clock(); 
    if(method == "Classic"){
        lloyds(clusters, numOfClusters, inputVectors, inputSize);
        outputfile<<"Algorithm: Lloyds"<<std::endl;
    }else if(method == "LSH")
    {
        reverse_approach_lsh(clusters, numOfClusters, input, L, kLSH, centroids.at(0).size(), euclidean);
        outputfile<<"Algorithm: LSH"<<std::endl;
    }
    else if(method == "Hypercube")
    {
        reverse_approach_hyper(clusters, numOfClusters, input, M, probes, kHYPER, euclidean);
        outputfile<<"Algorithm: Hypercube"<<std::endl;//Hypercube at clustering some times it seg faults
    }
    stop = clock();
    
    for(int i = 0; i < numOfClusters; i++){
        outputfile<<"CLUSTER-"<<i + 1<<" {size: "<<clusters[i].clusterVectors.size()<<", centroid:";
        for(std::vector<float>::iterator it = clusters[i].centroid.begin(); it != clusters[i].centroid.end(); it++){
            outputfile<<" "<<*it;
        }
        outputfile<<"}"<<std::endl;
    }
    tclustering = stop - start;
    outputfile<<"clustering_time: "<<(double)tclustering / CLOCKS_PER_SEC <<std::endl;
    silhouette(clusters, numOfClusters, inputVectors, inputSize, inVectors[0].size(), outputfile);//inVectors[0].size() is dimention
    

    if(completeFlag)
    {
        for(int i = 0; i <numOfClusters; i++)
        {
            outputfile<<"CLUSTER-"<<i + 1<<" {centroid:";
            for(std::vector<float>::iterator it = clusters[i].centroid.begin(); it != clusters[i].centroid.end(); it++){
             outputfile<<" "<<*it;
            }
            ;
            for(std::vector<HypercubeItem>::iterator it = clusters[i].clusterVectors.begin(); it != clusters[i].clusterVectors.end(); it++)
            {
                outputfile<<", "<< it->itemId;
            }
            outputfile<<"}"<<std::endl;
        }
    }

    delete[] inputVectors;

}