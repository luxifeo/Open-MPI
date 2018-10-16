#include <stdio.h>
#include <mpi.h>
#include <unistd.h>
int main(int argc, char **argv)
{
    /* code */
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    printf("Hello world from process %d of %d\n", rank, size);
    sleep(10);
    MPI_Finalize();
    return 0;
}
