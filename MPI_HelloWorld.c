#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv) {

	// Initialize MPI and check that the thread level asked for is provided
	int provided;
	MPI_Init_thread(&argc, &argv, MPI_THREAD_SERIALIZED, &provided);

	if (provided < MPI_THREAD_SERIALIZED) {
		printf("Thread level insufficient, exiting\n");
		abort();
	}

	// Find the process rank in the MPI_COMM_WORLD communicator
	int myid;
	MPI_Comm_rank(MPI_COMM_WORLD, &myid);

	// Find the totla number of procs in MPI_COMM_WORLD
	int num_procs;
	MPI_Comm_size(MPI_COMM_WORLD, &num_procs);


	MPI_Status status[2];
	MPI_Request request[2];
	int data_buf_send = 123;
	int data_buf_recv = 321;
	if (myid == 0) {
		data_buf_send = 777;
		// Send to 0th proc to first proc then recv
		MPI_Isend(&data_buf_send, 1, MPI_INT, 1, 7, MPI_COMM_WORLD, &request[0]);
		MPI_Irecv(&data_buf_recv, 1, MPI_INT, 1, 7, MPI_COMM_WORLD, &request[1]);
		// MPI_Send parameters: MPI_Send(void* data, int count, MPI_Datatype datatype, int destination, int tag, MPI_Comm communicator)
		// MPI_Recv parameters: MPI_Recv(void* data, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm communicator, MPI_Status* status)

	}
	else if (myid == 1) {
		data_buf_send = 999;
		// Send if we're 1st proc from the 0th proc then recv
		MPI_Isend(&data_buf_send, 1, MPI_INT, 0, 7, MPI_COMM_WORLD, &request[0]);
		MPI_Irecv(&data_buf_recv, 1, MPI_INT, 0, 7, MPI_COMM_WORLD, &request[1]);
	}
	else {
	}
	MPI_Waitall(2, request, status);

	printf("Hello World! I'm process %i of %i and my recv buffer is %d\n", myid, num_procs, data_buf_recv);

	// Finalize the MPI library
	MPI_Finalize();
}
