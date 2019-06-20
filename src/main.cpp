//
//  main.cpp
//  Hito1_SistemasDistribuidos
//
//  Created by Rafael Manzano on 8/10/18.
//  Copyright © 2018 Rafael Manzano. All rights reserved.
//

#include <iostream>
#include <vector>
#include "terminal.hpp"
#include "tree.hpp"
#include "mpi.h"

void master();
void slave(int rank);

int main(int argc, const char * argv[])
{
    int ndisks = 4;
	int size = 100; // ks

	int rank;
	int procCount;

	MPI_Init(NULL, NULL);	
	MPI_Comm_size(MPI_COMM_WORLD, &procCount);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	int world_size;
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);
	ndisks = world_size - 1;

	printf("Rank: %d\n ProcCount: %d\n world_size: %d\n",rank ,procCount,world_size);

	if(rank == 0){
		master();
	}
	else{
		slave(rank);
	}
    
	MPI_Finalize();
	return 0;

}

void master(){

	Terminal *terminal = new Terminal();
    

    std::cout << "Starting...\n";

    terminal->run();

}


void slave(int rank)
{
	DiskSlave* diskSlave = new DiskSlave(rank-1);
	diskSlave->listen();

}


