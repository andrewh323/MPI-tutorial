#define _Out_writes_(x)
#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
int provided;
MPI_Init_thread(&argc,&argv, MPI_THREAD_SERIALIZED, &provided);
if (provided < MPI_THREAD_SERIALIZED){
printf("Thread level insufficient, exiting\n");
abort();
}
printf("Hello World! \n");
MPI_Finalize();
}