#include <vector>
#include <cmath>
#include <fstream>
#include <string>
#include <iostream>
#include <random>
#include "../headers/HashTable.h"
#include "../headers/searchAlgorithms.h"
#include "../headers/util.h"


void lloyds(Cluster *clusters, unsigned numOfClusters, HypercubeItem *inputVectors, int inputSize)
{
    do
    {
        for(unsigned int i = 0; i < numOfClusters; i++){
            clusters[i].clusterVectors.clear();
        }
        for (int i = 0; i < inputSize; i++)
        {                                                                   //put vectors to right cluster (based on centroid)
            double minDistFromCluster = std::numeric_limits<double>::max();
            int indexClusterMinDist;

            for (unsigned int j = 0; j < numOfClusters; j++)
            {   
                
                double dist = euclidean(inputVectors[i].v, clusters[j].centroid);

                if (dist < minDistFromCluster)
                {
                    indexClusterMinDist = j;
                    minDistFromCluster = dist;
                }
            }
            clusters[indexClusterMinDist].clusterVectors.push_back(inputVectors[i]);
        }
        std::vector<std::vector<float>> centroids;
        centroids.clear();
        for (unsigned int i = 0; i < numOfClusters; i++)                                 //compute new centroids
        {
            std::vector<float> newCentroid;

            for (unsigned int dimention = 0; dimention < clusters[i].centroid.size(); dimention++)
            {
                float sum = 0;
                for (unsigned int everyVectorInCluster = 0; everyVectorInCluster < clusters[i].clusterVectors.size(); everyVectorInCluster++)
                {
                    sum += clusters[i].clusterVectors.at(everyVectorInCluster).v.at(dimention);
                }
                newCentroid.push_back(sum / (float)clusters[i].clusterVectors.size());
            }
            centroids.push_back(newCentroid);
        }

        bool cont = true;
        float tolerance = 10;

        for (unsigned int i = 0; i < numOfClusters; i++)         //if distance between previus centroid and current centroid  < tolerance stop lloyds algorith
        {

            if (euclidean(centroids.at(i), clusters[i].centroid) > tolerance)
            {
                cont = false;
            }
        }

        for (unsigned int i = 0; i < numOfClusters; i++)
        {
            clusters[i].centroid = centroids.at(i);
        }
        if (cont)
            break;

    } while (1);
    return;
}



void reverse_approach_lsh(Cluster *clusters, unsigned numOfClusters, std::vector<std::vector<float>> input, int L, int k, int dimention, float (*metric)(const std::vector<float> a, const std::vector<float> b)) //here input is as given with item_id inside
{
    float R;
    float minDistCentroids = std::numeric_limits<float>::max();             //calculate R
    for (unsigned int i = 0; i < numOfClusters; i++)
    {
        for (unsigned int j = i + 1; j < numOfClusters; j++)
        {
            float temp = metric(clusters[i].centroid, clusters[j].centroid);
            if (temp < minDistCentroids)
            {
                minDistCentroids = temp;
            }
        }
    }
    

    R = minDistCentroids / 2.0;
    std::map<float, int> marked_items; //-1 means a vector is not in a cluster, otherwise symbolises in which it is exp. 0 in claster 0, 1 in cluster 1 klpp <item_id, cluster>
    HashTable **tables = new HashTable *[L];        //create the hash tables

    for (int i = 0; i < L; i++)
    {
        tables[i] = new HashTable(input.size()/16, k, dimention);
    }


    for (std::vector<std::vector<float>>::iterator it = input.begin(); it != input.end(); it++)
    {
        marked_items[*(it->begin())] = -1;   //insert -1 to item with id it->begin(), initialization of marked items
        for (int i = 0; i < L; i++)
        {
            tables[i]->push(*it);
        }
    }
    

    int inputSize = input.size();
    int itemsInserted = 0;//if items inserted are >= 80% of all items, put the other items one by one
    int itemsInsertedBefore = 0;
    int counterForNoNewItemsInserted = 0; //counts how many iterations there are no new items inserted

    do{
        std::vector<std::vector<HashTableItem>> reslshRange;

        for (unsigned i = 0; i < numOfClusters; i++)            //range lsh search for every centroid
        {
            reslshRange.push_back(lsh_rangeSearch(clusters[i].centroid, tables, L, R, 1, metric));
        }

        int clusterIndex = 0;

        for(std::vector<std::vector<HashTableItem>>::iterator iterator1 = reslshRange.begin(); iterator1 != reslshRange.end(); iterator1++)
        {
            for(std::vector<HashTableItem>::iterator iterator2 = iterator1->begin(); iterator2 != iterator1->end(); iterator2++)
            {
                if(marked_items[iterator2->itemId] == -1){              //unmarked item
                    HypercubeItem temp;
                    temp.itemId = iterator2->itemId;
                    temp.v = iterator2->v;

                    clusters[clusterIndex].clusterVectors.push_back(temp);      //push item into the cluster
                    marked_items[iterator2->itemId] = clusterIndex;
                    itemsInserted++;
                }else{                                                  //markewd item
                    int previusCluster = marked_items[iterator2->itemId];

                    if(metric(iterator2->v, clusters[clusterIndex].centroid) < metric(iterator2->v, clusters[previusCluster].centroid))//if new distance <  previus distance update cluster
                    {
                        HypercubeItem temp;
                        temp.itemId = iterator2->itemId;
                        temp.v = iterator2->v;

                        clusters[previusCluster].clusterVectors.erase(find(clusters[previusCluster].clusterVectors.begin(), clusters[previusCluster].clusterVectors.end(), temp));
                        clusters[clusterIndex].clusterVectors.push_back(temp);
                        marked_items[iterator2->itemId] = clusterIndex;
                    }
                }
            }
            clusterIndex++;
            
        }
        if(itemsInserted == itemsInsertedBefore){       //if no new items inserted 5 continius times while R*=2 , stop LSH and put rest of items one by one
            counterForNoNewItemsInserted++;
            if(counterForNoNewItemsInserted >= 5) break;
        }else{
            counterForNoNewItemsInserted = 0;
        }

        itemsInsertedBefore = itemsInserted;
        //making new centroids
        for(unsigned i = 0; i < numOfClusters; i++){
            std::vector<float> newCentroid;
            for(int dim = 0; dim < dimention; dim++)                    //new centroid = average vector of vectors in a cluster
            {
                float sum = 0;
                for(unsigned int everyVectorInCluster = 0; everyVectorInCluster < clusters[i].clusterVectors.size(); everyVectorInCluster++){
                    sum += clusters[i].clusterVectors.at(everyVectorInCluster).v.at(dim);
                }
                newCentroid.push_back(sum / float(clusters[i].clusterVectors.size()));
            }
            clusters[i].centroid = newCentroid;
        }
        R*=2;
        
        //if itemsInserted stayed the same for five iterations break
    }while((float)itemsInserted/(float)inputSize < 0.8);


    //put rest items into clusters
    for (std::vector<std::vector<float>>::iterator it = input.begin(); it != input.end(); it++)
    {
        HypercubeItem temp;
        std::vector<float> currInVector(*it);
        std::vector<float>::iterator vectorIt = currInVector.begin();
        temp.itemId = *vectorIt;
        currInVector.erase(vectorIt);
        temp.v = currInVector;

        if(marked_items.at((float)temp.itemId) == -1)
        {
            float minDistFromCluster = std::numeric_limits<float>::max();
            int minCluster;

            for(unsigned int i = 0; i < numOfClusters; i++)
            {
                float dist = metric(temp.v, clusters[i].centroid);
                if(dist < minDistFromCluster){
                    minDistFromCluster = dist;
                    minCluster = i;
                }
            }

            clusters[minCluster].clusterVectors.push_back(temp);
        }
    }
    return;
}



void reverse_approach_hyper(Cluster *clusters, unsigned numOfClusters, std::vector<std::vector<float>> input, int M, int probes, int k, float (*metric)(const std::vector<float> a, const std::vector<float> b))
{

    float minDistCentroids = std::numeric_limits<float>::max();//find R
    for (unsigned int i = 0; i < numOfClusters; i++)
    {
        for (unsigned int j = i + 1; j < numOfClusters; j++)
        {
            float temp = metric(clusters[i].centroid, clusters[j].centroid);
            if (temp < minDistCentroids)
            {
                minDistCentroids = temp;
            }
        }
    }
    float R = minDistCentroids / 2.0;

    std::map<float, int> marked_items; //-1 means a vector is not in a cluster, otherwise symbolises in which it is exp. 0 in claster 0, 1 in cluster 1 klpp <item_id, cluster>
    for (std::vector<std::vector<float>>::iterator it = input.begin(); it != input.end(); it++)
    {
        marked_items[*(it->begin())] = -1;
    }

    

    int inputSize = input.size();           //size of input
    HypercubeItem inputVectors[inputSize];//here will be stored all input items
    int index = 0;
    unsigned dimention = input.begin()->size() - 1;

    Hypercube hypercube(k, dimention);      //making a hypercube
    

    for (std::vector<std::vector<float>>::iterator it = input.begin(); it != input.end(); it++)
    {
        HypercubeItem temp;
        std::vector<float> currInVector(*it);
        std::vector<float>::iterator vectorIt = currInVector.begin();
        temp.itemId = *vectorIt;
        currInVector.erase(vectorIt);
        temp.v = currInVector;
        inputVectors[index] = temp;
        index++;

        hypercube.push(*it);
    }


    int itemsInserted = 0;//if items inserted are >= 80% of all items, put the other items one by one
    int itemsInsertedBefore = 0;
    int counterForNoNewItemsInserted = 0; //counts how many iterations there are no new items inserted

    do{
        std::vector<std::vector<HypercubeItem>> reshyperRange;
        
        for (unsigned i = 0; i < numOfClusters; i++)        //hyper cube range search for all centroids
        {
            reshyperRange.push_back(hypercube_rangesearch(clusters[i].centroid, hypercube, M, probes, R, metric));
        }

        int clusterIndex = 0;
        for(std::vector<std::vector<HypercubeItem>>::iterator iterator1 = reshyperRange.begin(); iterator1 != reshyperRange.end(); iterator1++)
        {
            for(std::vector<HypercubeItem>::iterator iterator2 = iterator1->begin(); iterator2 != iterator1->end(); iterator2++)
            {
                if(marked_items[iterator2->itemId] == -1){              //unmarked item
                    HypercubeItem temp;
                    temp.itemId = iterator2->itemId;
                    temp.v = iterator2->v;

                    clusters[clusterIndex].clusterVectors.push_back(temp);  //push vector in right cluster
                    marked_items[iterator2->itemId] = clusterIndex;
                    itemsInserted++;
                }else{                                                  //markewd item
                    int previusCluster = marked_items[iterator2->itemId];

                    if(metric(iterator2->v, clusters[clusterIndex].centroid) < metric(iterator2->v, clusters[previusCluster].centroid))//if new distance <  previus distance update cluster
                    {
                        HypercubeItem temp;
                        temp.itemId = iterator2->itemId;
                        temp.v = iterator2->v;

                        clusters[previusCluster].clusterVectors.erase(find(clusters[previusCluster].clusterVectors.begin(), clusters[previusCluster].clusterVectors.end(), temp));
                        
                        
                        clusters[clusterIndex].clusterVectors.push_back(temp);
                        marked_items[iterator2->itemId] = clusterIndex;
                    }
                }
            }
            clusterIndex++;
            
        }

        if(itemsInserted == itemsInsertedBefore){       //if no new items inserted 5 continius times while R*=2 , stop LSH and put rest of items one by one
            counterForNoNewItemsInserted++;
            if(counterForNoNewItemsInserted >= 5) break;
        }else{
            counterForNoNewItemsInserted = 0;
        }
        itemsInsertedBefore = itemsInserted;


        //making new centroids
        for(unsigned i = 0; i < numOfClusters; i++){        //new centroid = average vector of vectors in a cluster
            std::vector<float> newCentroid;
            for(unsigned int dim = 0; dim < dimention; dim++)
            {
                float sum = 0;
                for(long unsigned int everyVectorInCluster = 0; everyVectorInCluster < clusters[i].clusterVectors.size(); everyVectorInCluster++){
                    sum += clusters[i].clusterVectors.at(everyVectorInCluster).v.at(dim);
                }
                newCentroid.push_back(sum / float(clusters[i].clusterVectors.size()));
            }
            clusters[i].centroid = newCentroid;
        }
        R*=2;
        
        //if itemsInserted stayed the same for five iterartions break
    }while((float)itemsInserted/(float)inputSize < 0.8);


    //put rest items into clusters
    for (std::vector<std::vector<float>>::iterator it = input.begin(); it != input.end(); it++)
    {
        HypercubeItem temp;
        std::vector<float> currInVector(*it);
        std::vector<float>::iterator vectorIt = currInVector.begin();
        temp.itemId = *vectorIt;
        currInVector.erase(vectorIt);
        temp.v = currInVector;

        if(marked_items.at((float)temp.itemId) == -1)
        {
            float minDistFromCluster = std::numeric_limits<float>::max();
            int minCluster;

            for(unsigned int i = 0; i < numOfClusters; i++)
            {
                float dist = metric(temp.v, clusters[i].centroid);
                if(dist < minDistFromCluster){
                    minDistFromCluster = dist;
                    minCluster = i;
                }
            }

            clusters[minCluster].clusterVectors.push_back(temp);
        }
    }

    return;

}