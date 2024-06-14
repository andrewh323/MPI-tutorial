#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv) {
	int provided;
	MPI_Init_thread(&argc, &argv, MPI_THREAD_SERIALIZED, &provided);
	if (provided < MPI_THREAD_SERIALIZED) {
		printf("Thread level insufficient, exiting\n");
		abort();
	}

	int myid;
	MPI_Comm_rank(MPI_COMM_WORLD, &myid);
	int num_procs;
	MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

	char dataset[100];
	char packed_buffer[120];
	int max_loc = 52;
	int position = 0;

	for (int i = 0; i < 99; i++) {
		dataset[i] = i;
	}

	int data_buf_send = 123;
	int data_buf_recv = 321;
	MPI_Status status;
	MPI_Request request;
	if (myid == 0) {
		dataset[max_loc] = 9;

		MPI_Pack(&max_loc, 1, MPI_INT, packed_buffer, 120, &position, MPI_COMM_WORLD);
		MPI_Pack(&dataset, 100, MPI_CHAR, packed_buffer, 120, &position, MPI_COMM_WORLD);

		MPI_Isend(&packed_buffer, 120, MPI_PACKED, 1, 7, MPI_COMM_WORLD, &request);
	}
	else if (myid == 1) {
		data_buf_send = 999;
		MPI_Irecv(packed_buffer, 120, MPI_PACKED, 0, 7, MPI_COMM_WORLD, &request);
		MPI_Wait(&request, &status);

		MPI_Unpack(packed_buffer, 120, &position, &max_loc, 1, MPI_INT, MPI_COMM_WORLD);
		MPI_Unpack(packed_buffer, 120, &position, dataset, 100, MPI_CHAR, MPI_COMM_WORLD);
	}
	else {
	}

	printf("Hello World! I'm process %i of %i and my recv buffer is %d at %d\n", myid, num_procs, dataset[max_loc], max_loc);

	MPI_Finalize();
}