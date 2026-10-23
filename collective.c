#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>
#include <stdint.h>

#define NUMSTEPS 500000000

int main(int argc, char *argv[]){
	int i;
	double x, pi;
	int rank, processes;
	double process_sum = 0.0;
	double final_sum = 0.0;
	struct timespec start, end;

	//initialize MPI environment
	MPI_Init(&argc, &argv);

	//get rank & num of processes
	MPI_Comm_size(MPI_COMM_WORLD, &processes);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);


	//calculate step
	double step = 1.0 / (double) NUMSTEPS;

	//start timer
	clock_gettime(CLOCK_MONOTONIC, &start);

	//process calculation
	for(i = rank; i < NUMSTEPS; i += processes){
		x = (i + 0.5) * step;
		process_sum += 4.0 / (1.0 + x*x);
	}

	//combines sums from all processes at rank 0
	MPI_Reduce(&process_sum, &final_sum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

	if(rank == 0){
		//calculate pi and end timer
		pi = step * final_sum;
		clock_gettime(CLOCK_MONOTONIC, &end);
		uint64_t diff = 1000000000 * (end.tv_sec - start.tv_sec) + end.tv_nsec - start.tv_nsec;
		
		//print pi and execution time
		printf("PI is %.20f\n", pi);
        	printf("elapsed time = %llu nanoseconds \n", (unsigned long long)diff);
	}

	//end MPI environment
	MPI_Finalize();
	return 0;

}
