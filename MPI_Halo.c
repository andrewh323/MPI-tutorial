#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

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

	//Find if num_procs is a perfect square
	long long sqrt_procs = (long long)round((sqrt(num_procs)));
	int global_x_procs, global_y_procs;
	if (sqrt_procs * sqrt_procs == num_procs) {
		global_x_procs = (int)sqrt_procs;
		global_y_procs = global_x_procs;
	}
	else {
		printf("This halo exchange only works on perfect square numbers of processes. %d is not a perfect square\n", num_procs);
		MPI_Finalize();
		exit(0);
	}

	//Set domain size for local proc, global domain will be num_procs
	//Arbitrary domain size for this example
	int face_size = 64;

	int local_domain[64][64];

	local_domain[0][0] = num_procs * myid;

	int data_buf_send[64];
	int data_buf_recv[64];

	//Setup MPI request/status as appropriate for the problem (2D or 3D)
	//Need 4 directions for 2D 5pt halo exchange
	MPI_Status status[8];
	MPI_Request request[8];

	//Declare variables for neighbour process rank finding
	int north_tag = 66;
	int east_tag = 77;
	int south_tag = 66;
	int west_tag = 77;
	int dest_north;
	if ((myid - global_x_procs) >= 0) {
		dest_north = myid - global_x_procs;
	}
	else {
		dest_north = myid;
	}

	int dest_east = 0;
	if ((myid + 1) < (global_y_procs - 1)) {
		dest_east = myid + 1;
	}
	else {
		dest_east = myid;
	}

	int dest_south = 0;
	if ((myid + global_x_procs) < num_procs) {
		dest_south = myid + global_x_procs;
	}
	else {
		dest_south = myid;
	}

	int dest_west = 0;
	if ((myid % global_y_procs - 1) >= 0) {
		dest_west = myid - 1;
	}
	else {
		dest_west = myid;
	}


	int source_north = dest_north;
	int source_east = dest_east;
	int source_south = dest_south;
	int source_west = dest_west;

	source_north = ((myid - global_x_procs) >= 0) ? myid - global_x_procs : myid;
	printf("north source for proc %d is %d\n", myid, source_north);
	source_east = ((myid + 1) < (global_y_procs - 1)) ? myid + 1 : myid;
	printf("east source for proc %d is %d\n", myid, source_east);


	for (int iter_count = 0; iter_count <= 10; iter_count++) {

		//Exchange North
		MPI_Isend(&data_buf_send, face_size, MPI_INT, dest_north, north_tag, MPI_COMM_WORLD, &request[0]);
		MPI_Irecv(&data_buf_recv, face_size, MPI_INT, source_north, south_tag, MPI_COMM_WORLD, &request[1]);
		if (myid == 0) {
			printf("Done North exchange in loop #%d\n", iter_count);
		}

		//Exchange East
		MPI_Isend(&data_buf_send, face_size, MPI_INT, dest_east, east_tag, MPI_COMM_WORLD, &request[2]);
		MPI_Irecv(&data_buf_recv, face_size, MPI_INT, source_east, west_tag, MPI_COMM_WORLD, &request[3]);
		if (myid == 0) {
			printf("Done East exchange in loop #%d\n", iter_count);
		}

		//Exchange South
		MPI_Isend(&data_buf_send, face_size, MPI_INT, dest_south, south_tag, MPI_COMM_WORLD, &request[4]);
		MPI_Irecv(&data_buf_recv, face_size, MPI_INT, source_south, north_tag, MPI_COMM_WORLD, &request[5]);
		if (myid == 0) {
			printf("Done South exchange in loop #%d\n", iter_count);
		}

		//Exchange West
		MPI_Isend(&data_buf_send, face_size, MPI_INT, dest_west, west_tag, MPI_COMM_WORLD, &request[6]);
		MPI_Irecv(&data_buf_recv, face_size, MPI_INT, source_west, east_tag, MPI_COMM_WORLD, &request[7]);
		if (myid == 0) {
			printf("Done West exchange in loop #%d\n", iter_count);
		}

		MPI_Waitall(7, request, status);

		MPI_Reduce(local_domain, data_buf_recv, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
		if (myid == 0) {
			printf("reduction: %d\n", data_buf_recv[0]);
			local_domain[0][0]++;
		}

	}
	MPI_Finalize();
}