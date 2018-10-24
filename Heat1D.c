#include <stdio.h>
#include <mpi.h>
#include <malloc.h>
#define M 20
#define Time 10
#define dt 0.01
#define dx 0.1
#define D 0.1
#define tagI 1 // Data Initializer
#define tagL 2 // Left point
#define tagR 3 // Right point
#define tagC 4 // Combine
void Write2File(float *T, int size)
{
    FILE *result = fopen("He_1DC.txt", "a");
    int i;
    fprintf(result, "Called one\n");
    for (i = 0; i < size; i++)
    {
        fprintf(result, "%lf", *(T + i));
        fprintf(result, "\n");
    }
    fclose(result);
}
void KhoiTao(float *T)
{
    int i;
    for (i = 0; i < M; i++)
        *(T + i) = 25.0;
    Write2File(T, M);
}
void Daoham(float *Tf, float *dTf, int length, float Tl, float Tr)
{
    int i;
    float c, l, r;
    for (i = 0; i < length; i++)
    {
        c = *(Tf + i);
        l = (i == 0) ? Tl : *(Tf + (i - 1));
        r = (i == length - 1) ? Tr : *(Tf + (i + 1));
        *(dTf + i) = (r - 2 * c + l) / (dx * dx);
    }
}
void DisplayArray(float *T, int size)
{
    int i;
    for (i = 0; i < size; i++)
        printf("  %.2f", *(T + i));
    printf("\n");
}
/*
    The start point of a partition
*/
int start(int rank, int size, float avg)
{
    return (int)(avg * rank);
}
/*
    The end point of a partition, but does not belong to, which means
    The start point of the next partition
*/
int end(int rank, int size, float avg)
{
    return (int)(avg * (rank + 1));
}
/*
    The size of a partition
*/
int range(int rank, int size, float avg)
{
    return end(rank, size, avg) - start(rank, size, avg);
}
int main(int argc, char **argv)
{
    int rank, size;
    float *T, *dT;
    T = (float *)malloc((M) * sizeof(float));
    dT = (float *)malloc((M) * sizeof(float));
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Status stat;
    float avg = M / (float)size;
    float *Tf; // T's fraction
    float Tl, Tr;
    float *dTf;
    Tf = (float *)malloc((range(rank, size, avg)));
    dTf = (float *)malloc((range(rank, size, avg)));
    if (rank == 0)
    {
        KhoiTao(T);
        Tl = 100.0;
        for (int i = 0; i < range(rank, size, avg); i++)
            Tf[i] = T[i];
        for (int i = 1; i < size; i++)
            MPI_Send(T + start(i, size, avg), range(i, size, avg), MPI_FLOAT, i, tagI, MPI_COMM_WORLD);
    }
    else
    {
        MPI_Recv(Tf, range(rank, size, avg), MPI_FLOAT, 0, tagI, MPI_COMM_WORLD, &stat);
        if (rank == size - 1)
        {
            Tr = 30.0;
        }
    }
    int ntime = Time / dt;
    for (int i = 0; i < ntime; i++)
    {
        if (rank == 0)
        {
            MPI_Send(Tf + end(rank, size, avg) - 1, rank + 1, MPI_FLOAT, rank + 1, tagL, MPI_COMM_WORLD);
            MPI_Recv(&Tr, 1, MPI_FLOAT, rank + 1, tagR, MPI_COMM_WORLD, &stat);
        }
        else
        {
            MPI_Send(Tf, 1, MPI_FLOAT, rank - 1, tagR, MPI_COMM_WORLD);
            MPI_Recv(&Tl, 1, MPI_FLOAT, rank - 1, tagL, MPI_COMM_WORLD, &stat);
            if (rank != size - 1)
            {
                MPI_Send(Tf + range(rank, size, avg) - 1, 1, MPI_FLOAT, rank + 1, tagL, MPI_COMM_WORLD);
                MPI_Recv(&Tr, 1, MPI_FLOAT, rank + 1, tagR, MPI_COMM_WORLD, &stat);
            }
        }
        MPI_Barrier(MPI_COMM_WORLD);
        Daoham(Tf, dTf, range(rank, size, avg), Tl, Tr);
        for (int i = 0; i < range(rank, size, avg); i++)
        {
            *(Tf + i) = *(Tf + i) + D * dt * (*(dTf + i));
        }
        // Combine data
        if (rank == 0)
        {
            for (int i = 0; i < range(rank, size, avg); i++)
            {
                T[i] = Tf[i];
            }
            for (int i = 1; i < size; i++)
            {
                MPI_Recv(T + start(i, size, avg), range(i, size, avg), MPI_FLOAT, i, tagC, MPI_COMM_WORLD, &stat);
            }
            Write2File(T, M);
        }
        else
        {
            MPI_Send(Tf, range(rank, size, avg), MPI_FLOAT, 0, tagC, MPI_COMM_WORLD);
        }
        MPI_Barrier(MPI_COMM_WORLD);
    }
    if (rank == 0)
    {
        for (int i = 0; i < M; i++)
        {
            printf("  %.2f", T[i]);
        }
        printf("\n");
    }
    MPI_Finalize();
}
