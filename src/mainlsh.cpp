#include <iostream>
#include <cstring>
#include <string>
#include <fstream>
#include <ctime>
#include "../headers/HashTable.h"
#include "../headers/searchAlgorithms.h"
#include "../headers/util.h"
//if too little with same ID found then search the whole bucket of every hash table
//numOfBuckets n/16
int main(int argc, char *argv[])
{
    bool iFoundFlag = false, qFoundFlag = false, oFoundFlag = false;
    int k = 4, L = 5, N = 1, R = 10000;
    std::string flags[7] = {"-i", "-q", "-k", "-L", "-o", "-N", "-R"};
    std::string inFile, outFile, queryFile;
    
    for (int i = 1; i < argc; i += 2)                   //parsing arguments form cmd
    {
        int flagsIndex = -1;
        if (argc <= i + 1)
        {
            return 1; //error, expected something after flag, form = (-something something)
        }
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
            queryFile = std::string(argv[i + 1]);
            qFoundFlag = true;
            break;
        case 2:
            k = atoi(argv[i + 1]);
            break;
        case 3:
            L = atoi(argv[i + 1]);
            break;
        case 4:
            outFile = std::string(argv[i + 1]);
            oFoundFlag = true;
            break;
        case 5:
            N = atoi(argv[i + 1]);
            break;
        case 6:
            R = atoi(argv[i + 1]);
            break;
        }
    }                                                   //parsing arguments form cmd

    if (!oFoundFlag || !iFoundFlag || !qFoundFlag)
    {
        std::cout<<"input file or output file or query file not given, program finished!"<<std::endl;
        return 1; //error not given the files
    }

    std::vector<std::vector<float>> input = inputFileParser(inFile);//input contains all input items as vectors with theis itemid inside
    int inputSize = input.size();                                   //how many input vectors i got
    HypercubeItem *inputVectors = new HypercubeItem[inputSize];                          //input vecors will contain all input vectors with form of HypercubeItems array
    int index = 0;
    unsigned dimention = input.begin()->size() - 1;                 //dimention of vector input


    HashTable **tables = new HashTable *[L];
    for (int i = 0; i < L; i++)                                     //making L hash tables
    {
        tables[i] = new HashTable(inputSize/16, k, dimention);
    }

    for (std::vector<std::vector<float>>::iterator it = input.begin(); it != input.end(); it++)//insert input at the hash tables, and at inputVectors
    {
        HypercubeItem temp;
        std::vector<float> currInVector(*it);
        std::vector<float>::iterator vectorIt = currInVector.begin();
        temp.itemId = *vectorIt;
        currInVector.erase(vectorIt);
        temp.v = currInVector;
        inputVectors[index] = temp;
        index++;

        for (int i = 0; i < L; i++)
        {
            tables[i]->push(*it);
        }
    }

    clock_t start, stop, tlsh, tbruteforce;
    std::ofstream outputfile(outFile);
    float *distancesLSH = new float[N];
    float *distancesTRUE = new float[N];
    std::vector<std::vector<float>> queries = inputFileParser(queryFile);       //queris contain all the quieries as vector with their id inside the vector

    for (std::vector<std::vector<float>>::iterator it = queries.begin(); it != queries.end(); it++)
    {
        HypercubeItem temp;
        std::vector<float> currQueryVector(*it);
        std::vector<float>::iterator vectorIt = currQueryVector.begin();
        temp.itemId = *vectorIt;
        currQueryVector.erase(vectorIt);
        temp.v = currQueryVector;


        start = clock();                                                       //start clock, do LSH for a queryVector, stop clock when LSH is finished
        HashTableItem *reslsh = lsh(temp.v, tables, L, N, distancesLSH, &euclidean);
        stop = clock();
        tlsh = stop - start;

        start = clock();                                                        //start clock, do true_distance for a queryVector, stop clock when true_distance algorithm is finished 
        true_distance_NN(temp.v, inputVectors, inputSize, N, distancesTRUE, &euclidean);
        stop = clock();
        tbruteforce = stop - start;


        std::vector<HashTableItem> reslshRange = lsh_rangeSearch(temp.v, tables, L, R, 0,  &euclidean); //do lsh_rangesearch for a queryVectror

        outputfile << "Query: " << temp.itemId << std::endl;                //print what the assigment says 
        for (int i = 0; i < N; i++)
        {
            outputfile << "Nearest neighbor-" << i + 1 << ": " << reslsh[i].itemId << std::endl;
            outputfile << "distanceLSH: " << distancesLSH[i] << std::endl;
            outputfile << "distanceTRUE: " << distancesTRUE[i] << std::endl;
        }
        outputfile << "tLSH: " << (double)tlsh / CLOCKS_PER_SEC << std::endl;
        outputfile << "tTrue: " << (double)tbruteforce / CLOCKS_PER_SEC << std::endl;
        outputfile << "R-near neighbors:" << std::endl;
        for (std::vector<HashTableItem>::iterator it = reslshRange.begin(); it != reslshRange.end(); it++)
        {
            outputfile << "item_id: " << it->itemId << std::endl;
        }

        delete[] reslsh;
    }

    delete[] inputVectors;
    delete[] distancesLSH;
    delete[] distancesTRUE;
    delete[] tables;
    ///SOS MUST ASK USER IF HE WANTS TO CONTINIUE
    return 0;
}