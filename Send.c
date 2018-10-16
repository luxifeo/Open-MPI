#include <stdio.h>
#include <mpi.h>
#include <unistd.h>
#define MAX 10
int main(int argc, char **argv)
{
    /* code */
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Status stat;
    int s[MAX] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    int r[MAX];
    for (int j = 0; j < MAX; j++)
    {
        r[j] = 0;
    }
    if (rank == 0)
        for (int j = 1; j < size; j++)
            MPI_Send(s + (j - 1) * 5, 5, MPI_INT, j, 0, MPI_COMM_WORLD);
    else MPI_Recv(r, 5, MPI_INT, 0, 0, MPI_COMM_WORLD, &stat);
    if (rank != 0)
    {
        printf("process %d received: ", rank);
        for (int i = 0; i < 5; i++)
        {
            printf("%d ", r[i]);
        }
        printf("\n");
        MPI_Send(r, 5, MPI_INT, 0, 1, MPI_COMM_WORLD);
    }
    if(rank == 0) {
        MPI_Recv(r, 5, MPI_INT, 1, 1, MPI_COMM_WORLD, &stat);
        MPI_Recv(r+5, 5, MPI_INT, 2, 1, MPI_COMM_WORLD, &stat);
        printf("Hehehe i dun know what is wrong if this is correct xD\n");
        for(int i = 0; i < 10; i++) {
            printf("%d ", r[i]);
        }
        printf("\n");
    }
    MPI_Finalize();
    return 0;
}
