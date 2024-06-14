#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv) {
	const int PING_PONG_LIST = 10;
	// Initialize MPI and check that the thread level asked for is provided
	int provided;
	MPI_Init_thread(&argc, &argv, MPI_THREAD_SERIALIZED, &provided);

	if (provided < MPI_THREAD_SERIALIZED) {
		printf("Thread level insufficient, exiting\n");
		abort();
	}

	// Find the process rank in the MPI_COMM_WORLD communicator
	int world_rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
	int world_size;
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);
	
	// Assume 2 processes
	if (world_size != 2) {
		fprintf(stderr, "World size must be two for %s\n", argv[0]);
		MPI_Abort(MPI_COMM_WORLD, 1);
	}

	int ping_pong_count = 0;
	int partner_rank = (world_rank + 1) % 2;
	MPI_Status status[1];

	while (ping_pong_count < PING_PONG_LIST) {
		if (world_rank == ping_pong_count % 2) {
			ping_pong_count++;
			MPI_Send(&ping_pong_count, 1, MPI_INT, partner_rank, 0, MPI_COMM_WORLD);
			printf("%d sent and incremented ping_pong_count %d to %d\n", world_rank, ping_pong_count, partner_rank);
		}
		else {
			MPI_Recv(&ping_pong_count, 1, MPI_INT, partner_rank, 0, MPI_COMM_WORLD, &status);
			printf("%d received ping_pong_count %d from %d\n", world_rank, ping_pong_count, partner_rank);
		}
	}

	// Finalize the MPI library
	MPI_Finalize();
}
