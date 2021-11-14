#include <iostream>
#include <cstring>
#include <fstream>
#include "../headers/Hypercube.h"
#include "../headers/searchAlgorithms.h"
#include "../headers/util.h"


int main(int argc, char const *argv[])
{
bool iFoundFlag = false, qFoundFlag = false, oFoundFlag = false;
    int k = 14, M = 10, N = 1, R = 10000, probes = 2;
    std::string flags[8] = {"-i", "-q", "-k", "-M", "-probes", "-o", "-N", "-R"};
    std::string inFile, outFile, queryFile;

    for (int i = 1; i < argc; i += 2)                        //parsing arguments form cmd
    {
        int flagsIndex = -1;
        if (argc <= i + 1)
        {
            return 1; //error, expected something after flag, form = (-something something) 
        }
        for (int j = 0; j < 8; j++)
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
            M = atoi(argv[i + 1]);
            break;
        case 4:
            probes = atoi(argv[i + 1]);
            break;
        case 5:
            outFile = std::string(argv[i + 1]);
            oFoundFlag = true;
            break;
        case 6:
            N = atoi(argv[i + 1]);
            break;
        case 7:
            R = atoi(argv[i + 1]);
            break;
        }
    }

    if (!oFoundFlag || !iFoundFlag || !qFoundFlag)          //parsing arguments form cmd
    {
        std::cout << "Input file or query file or output file not given, try again!" << std::endl;
        return 1; //error not given the files
    }

    
    std::vector<std::vector<float>> input = inputFileParser(inFile);    //input contains all input items as vectors with theis itemid inside
    int inputSize = input.size();                                           //how many input vectors i got
    HypercubeItem *inputVectors = new HypercubeItem[inputSize];                                  //input vecors will contain all input vectors with form of HypercubeItems array
    int index = 0;
    unsigned dimention = input.begin()->size() - 1;                         //dimention of vector input


    Hypercube hypercube(k, dimention);                                  //making of the Hypercube
    

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

        hypercube.push(*it);
    }

    clock_t start, stop, tHYPER, tbruteforce;
    std::ofstream outputfile(outFile);
    float *distancesHYPER = new float[N];
    float *distancesTRUE = new float[N];

    std::vector<std::vector<float>> queries = inputFileParser(queryFile);//queris contain all the quieries as vector with their id inside the vector

    for (std::vector<std::vector<float>>::iterator it = queries.begin(); it != queries.end(); it++)
    {
        HypercubeItem temp;
        std::vector<float> currQueryVector(*it);
        std::vector<float>::iterator vectorIt = currQueryVector.begin();
        temp.itemId = *vectorIt;
        currQueryVector.erase(vectorIt);
        temp.v = currQueryVector;


        start = clock();             //start clock, do Hypercube for a queryVector, stop clock when Hypercube is finished
        HypercubeItem *reshyper = hypercube_search(temp.v, hypercube, M, probes, N, distancesHYPER, &euclidean); 
        stop = clock();
        tHYPER = stop - start;
        
        start = clock();            //start clock, do true_distance for a queryVector, stop clock when true_distance algorithm is finished 
        true_distance_NN(temp.v, inputVectors, inputSize, N, distancesTRUE, &euclidean);
        stop = clock();
        tbruteforce = stop-start;
        std::vector<HypercubeItem> reshyperRange = hypercube_rangesearch(temp.v, hypercube, M, probes, R,  &euclidean);//do Hypercube_range_search for queryVector

        
        
        outputfile << "Query: " << temp.itemId << std::endl;     //print what the assigment says 
        for (int i = 0; i < N; i++)
        {
            outputfile << "Nearest neighbor-" << i + 1 << ": " << reshyper[i].itemId << std::endl;
            outputfile << "distanceHYPER: " << distancesHYPER[i] << std::endl;
            outputfile << "distanceTRUE: " << distancesTRUE[i] << std::endl;
        }

        outputfile << "tHYPER: " << (double)tHYPER / CLOCKS_PER_SEC << std::endl;
        outputfile << "tTrue: " << (double)tbruteforce / CLOCKS_PER_SEC << std::endl;
        outputfile << "R-near neighbors:" << std::endl;
        for (std::vector<HypercubeItem>::iterator it = reshyperRange.begin(); it != reshyperRange.end(); it++)
        {
            outputfile << "item_id: " << it->itemId << std::endl;
        }
        outputfile << std::endl;
        delete[] reshyper;
    
    }

    delete[] inputVectors;
    delete[] distancesHYPER;
    delete[] distancesTRUE;
    ///SOS MUST ASK USER IF HE WANTS TO CONTINIUE
    return 0;

}
