#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_NUMBERS 100

int main(int argc, char* argv[])
{
    // Initialization
    int provided;
    MPI_Init_thread(&argc, &argv, MPI_THREAD_SERIALIZED, &provided);
    
    int size;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    if (size != 2) {
        fprintf(stderr, "Must use two processes\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int number_amount;
    if (rank == 0) {
        int numbers[MAX_NUMBERS];
        // Generate random amount of numbers to send to process 1
        srand(time(NULL));
        number_amount = rand() % (MAX_NUMBERS + 1);
        // Send the integers to process 1
        MPI_Send(numbers, number_amount, MPI_INT, 1, 0, MPI_COMM_WORLD);
        printf("0 sent %d numbers to 1\n", number_amount);
    }
    else if (rank == 1) {
        MPI_Status status;
        // Probe for a message coming from process 0
        MPI_Probe(0, 0, MPI_COMM_WORLD, &status);
        // When probe returns, retrieve the size of the message from status
        MPI_Get_count(&status, MPI_INT, &number_amount);
        // Allocate a buffer to hold the incoming numbers
        int* number_buf = (int*)malloc(sizeof(int) * number_amount);
        // Receive the message using the allocated buffer
        MPI_Recv(number_buf, number_amount, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("1 dynamically received %d numbers from 0\n", number_amount);
        free(number_buf);
    }

    MPI_Finalize();
}
