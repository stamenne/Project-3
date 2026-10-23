#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>
#include <stdint.h>

#define NUMSTEPS 1000000

int main(int argc, char *argv[]){
        int i;
        double x, pi;
        int rank, processes;
        double process_sum = 0.0;
        double final_sum = 0.0;
        double received_sum;
        struct timespec start, end;

        //initialize MPI environment
        MPI_Init(&argc, &argv);

        //get rank & number of processes
        MPI_Comm_size(MPI_COMM_WORLD, &processes);
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);

        //calculate step
        double step = 1.0/(double) NUMSTEPS;

        //start timer
        clock_gettime(CLOCK_MONOTONIC, &start);

        //process calculation
        for(i=rank; i<NUMSTEPS; i+= processes){
                x = (i+0.5) * step;
                process_sum += 4.0 / (1.0 + x*x);
        }

        if(rank != 0) {
                //all processes send sum to rank 0
                MPI_Send(&process_sum, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
        }
        else{
                //start with rank 0 local sum
                final_sum = process_sum;

                //get sums from all other processes.
                for(int source = 1; source < processes; source++){
                        MPI_Recv(&received_sum, 1, MPI_DOUBLE, source, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                        final_sum += received_sum;
                }


	//calculate pi
        pi = step * final_sum;

	//stop timer
        clock_gettime(CLOCK_MONOTONIC, &end);

        uint64_t diff = 1000000000ULL * (end.tv_sec - start.tv_sec) + end.tv_nsec - start.tv_nsec;


	//print pi & execution time
        printf("PI is %.20f\n", pi);
        printf("elapsed time = %llu nanoseconds \n", (unsigned long long)diff);

        }

	//end MPI environment
        MPI_Finalize();
        return 0;
}

