
CC=g++
CFLAGS=-g -Wall
# source code directory name
SRC=src
# object file directory name
OBJ=obj
# list of source files
SRCS=$(wildcard $(SRC)/*.cpp)
# list of object files
OBJS=$(patsubst $(SRC)/%.cpp, $(OBJ)/%.o, $(SRCS))
# binary file directory name
BINDIR=bin
# executable file name
BIN1=$(BINDIR)/lsh
BIN2=$(BINDIR)/cube
BIN3=$(BINDIR)/cluster

all:$(BIN1) $(BIN2) $(BIN3)

$(BIN1): $(OBJ)/mainlsh.o $(OBJ)/HashTable.o $(OBJ)/Hypercube.o $(OBJ)/searchAlgorithms.o $(OBJ)/util.o
	$(CC) $(CFLAGS) $(OBJ)/mainlsh.o $(OBJ)/HashTable.o $(OBJ)/Hypercube.o $(OBJ)/searchAlgorithms.o $(OBJ)/util.o -o $@

$(BIN2): $(OBJ)/mainhyper.o $(OBJ)/HashTable.o $(OBJ)/Hypercube.o $(OBJ)/searchAlgorithms.o $(OBJ)/util.o
	$(CC) $(CFLAGS) $(OBJ)/mainhyper.o $(OBJ)/HashTable.o $(OBJ)/Hypercube.o $(OBJ)/searchAlgorithms.o $(OBJ)/util.o -o $@

$(BIN3): $(OBJ)/maincluster.o $(OBJ)/HashTable.o $(OBJ)/Hypercube.o $(OBJ)/searchAlgorithms.o $(OBJ)/util.o $(OBJ)/clusteringAlgos.o
	$(CC) $(CFLAGS) $(OBJ)/maincluster.o $(OBJ)/HashTable.o $(OBJ)/Hypercube.o $(OBJ)/searchAlgorithms.o $(OBJ)/util.o $(OBJ)/clusteringAlgos.o -o $@

$(OBJ)/%.o: $(SRC)/%.cpp
	$(CC) $(CFLAGS) -std=c++11 -c $< -o $@

clean:
	$(RM) -r $(BINDIR)/* $(OBJ)/*