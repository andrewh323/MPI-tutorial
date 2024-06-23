#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>


int main(int argc, char* argv[])
{
    // Initialization
    int provided;
    MPI_Init_thread(&argc, &argv, MPI_THREAD_SERIALIZED, &provided);
    
    int rank, size, next, prev;
    int message;

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Calculate next and previous process ranks
    next = (rank + 1) % size;
    prev = (rank + size - 1) % size;

    // If process 0, send the message to process 1
    if (0 == rank) {
        message = 10;
        MPI_Send(&message, 1, MPI_INT, next, 0, MPI_COMM_WORLD);
    }

    // Pass message around the ring. Decrement message each time it passes process 0
    // If a process receives a message containing 0, it sends the message and quits
    while (1) {
        MPI_Recv(&message, 1, MPI_INT, prev, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        if (0 == rank) {
            --message;
        }
        MPI_Send(&message, 1, MPI_INT, next, 0, MPI_COMM_WORLD);
        if (0 == message) {
            break;
        }
    }

    // The last proces does a final send to process 0, which must be received before the program can exit
    if (0 == rank) {
        MPI_Recv(&message, 1, MPI_INT, prev, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    fprintf(stderr, "Rank %d received message %d from process %d and completed ring\n", rank, message, prev);
    MPI_Barrier(MPI_COMM_WORLD);
    fprintf(stderr, "Rank %d has completed MPI_Barrier\n", rank);
    MPI_Finalize();
}
