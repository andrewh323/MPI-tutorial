#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

	int buf_size = 4;
	int *data_buf_send = (int*)malloc((buf_size * num_procs + 1) * sizeof(int));
	if (data_buf_send == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
    }

	int *data_buf_recv = (int*)malloc((buf_size * num_procs + 1) * sizeof(int));
	if (data_buf_recv == NULL) {
		fprintf(stderr, "Memory allocation failed\n");
		MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
	}

	memset(data_buf_recv, 0, (num_procs*buf_size)*sizeof(int));

	MPI_Status status[3];
	MPI_Request request[3];

	int *example_data = (int*)malloc((buf_size * num_procs + 1) * sizeof(int));
	for (int i = 0; i <= buf_size*num_procs; i++) {
		example_data[i] = myid + i;
	}

	printf("Process %d of %d, local values = %d, %d, %d, %d\n", myid, num_procs, example_data[0], example_data[1], example_data[2], example_data[3]);

	MPI_Allreduce(example_data, data_buf_recv, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);

	printf("Hello Collectives! I'm process %i of %i and my recv buffer is %d %d %d %d %d %d\n", myid, num_procs, data_buf_recv[0], data_buf_recv[1], data_buf_recv[2], data_buf_recv[3], data_buf_recv[4], data_buf_recv[5]);

	free(data_buf_send);
	free(data_buf_recv);
	free(example_data);

	MPI_Finalize();
}