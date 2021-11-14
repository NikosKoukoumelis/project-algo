Nikolaos Koukoumelis, AM: 1115201800082
Vasileios Sanidas, AM: 1115201800170

Aproximate NN, kmeans and clustering using different methods, with input some vectors with real number.

Our project approaches the problem of Nearest Neighbors, with two different methods (aNN): LSH, Hypercube and for comparison the brute force way which finds the true-NN.

Also the project approaches the problem of clustering ; kmeans++ algorithms is implimented for centroid
initialization at clustering problem.LSH and Hypercube aNN techniques are being use to clustering,
while a brute force way Lloyd's algorithm is also being used for comparation.

The source code files are located in the src/ directory:
    clusteringAlgos.cpp : contains the methods for clustering (Lloyd, reverse approach with (i)LSH, (ii)Hypercube).
    HashTable.cpp : implementation of the hash table class, used by lsh algorith.
    Hypercube.cpp : implementation of the hypercube class, used vy Hypercube algorithm.
    maincluster.cpp : main used by ./bin/cluster binary file.
    mainhyper.cpp : main used by ./bin/cube binary file.
    mainlsh.cpp : main used by ./bin/lsh binary file.
    searchAlgorithms.cpp : contains methods for the NN problem.
    util.cpp : contains utilities used in defferent parts of the program.


kmeansplusplus function and sihlouette functions are located to util.cpp


To compile type the following commands:
~/project-algo$ make clean
~/project-algo$ make

To execute type the following commands:

~/project-algo$ ./bin/cube –i <input file> –q <query file> –k <int> -M <int> -probes <int> -ο <output file> -Ν <number ofnearest> -R <radius>
    
~/project-algo$ ./bin/lsh –i <input file> –q <query file> –k <int> -L <int> -ο <output file> -Ν <number of nearest> -R <radius>
    

~/project-algo$ ./bin/cluster –i <input file> –c <configuration file> -o <output file> -complete <optional> -m <method: Classic OR LSH or Hypercube>

Input files and query files are not in included in the project-algo folder.