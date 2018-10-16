#include <stdio.h>
#include <mpi.h>
#include <malloc.h>
#define M 30
#define tagA 1
#define tagB 2
#define tagC 3
// This program will work with 4 processes, 1 master and 3 slaves

int main(int argc, char **argv)
{
    int rank, size;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Status stat;
    if (rank == 0) // Master process initializes data, sends a portion to other threads and waiting for results
    {
        int i, *A, *B, *C;
        A = (int *)malloc(M * sizeof(int));
        B = (int *)malloc(M * sizeof(int));
        for (i = 0; i < M; i++)
        {
            *(A + i) = i;
            *(B + i) = 2 * i;
        }
        for (i = 1; i < size; i++)
        {
            //printf("Sending to process %d\n", i);
            MPI_Send(A + (i - 1) * 10, 10, MPI_INT, i, tagA, MPI_COMM_WORLD);
            MPI_Send(B + (i - 1) * 10, 10, MPI_INT, i, tagB, MPI_COMM_WORLD);
        }
        free(A);
        free(B);
        C = (int *)malloc(M * sizeof(int));
        for (i = 1; i < size; i++)
        {
            MPI_Recv(C + (i - 1) * 10, 10, MPI_INT, i, tagC, MPI_COMM_WORLD, &stat);
            printf("Data received from process %d\n", i);
        }
        printf("Result");
        for (i = 0; i < M; i++)
        {
            if(i % 10 == 0)
                printf("\n");
            printf("%4d", C[i]);
        }
        printf("\n");
        free(C);
    }
    if (rank != 0)
    {
        int *D, *E, *F;
        D = (int *)malloc(10 * sizeof(int));
        E = (int *)malloc(10 * sizeof(int));
        F = (int *)malloc(10 * sizeof(int));
        //printf("Process %d received data\n", rank);
        MPI_Recv(D, 10, MPI_INT, 0, tagA, MPI_COMM_WORLD, &stat);
        MPI_Recv(E, 10, MPI_INT, 0, tagB, MPI_COMM_WORLD, &stat);
        for (int j = 0; j < 10; j++)
        {
            *(F + j) = *(D + j) + *(E + j);
        }
        // for (int k = 0; k < 10; k++)
        //     printf("%d ", F[k]);
        MPI_Send(F, 10, MPI_INT, 0, tagC, MPI_COMM_WORLD);
        free(D);
        free(E);
        free(F);
        // printf("\nData sent\n");
    }
    MPI_Finalize();
    return 0;
}
