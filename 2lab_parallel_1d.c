#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

#define ISIZE 1000
#define JSIZE 1000

int main(int argc, char **argv)
{
    double* a = (double*)malloc(ISIZE * JSIZE * sizeof(double));
    int i, j;

    FILE *ff;
    for (i=0; i<ISIZE; i++)
    {
        for (j=0; j<JSIZE; j++)
        {
            a[i * JSIZE + j] = 10 * i + j;
        }
    }


    MPI_Init(&argc, &argv);
    double start_t = MPI_Wtime();

    int num_proc, proc_id;

    MPI_Comm_size(MPI_COMM_WORLD, &num_proc);
    MPI_Comm_rank(MPI_COMM_WORLD, &proc_id);

    int start, finish;
    if (proc_id == 0)
    {
        start = 1;
    }
    else
    {
        start = proc_id * (ISIZE / num_proc);
    }
    
    if (proc_id == num_proc - 1)
    {
        finish = ISIZE - 1;
    }
    else
    {
        finish = start + (ISIZE / num_proc);
    }

    for (i = start; i < finish; i++)
    {
        for (j = 6; j < JSIZE - 1; j++)
        {
            a[i * JSIZE + j] = sin(0.00001 * a[(i + 1) * JSIZE + j - 6]);
        }
    }

    MPI_Status status;

    if (proc_id != 0)
    {
        MPI_Send(&a[start * JSIZE], (finish - start) * JSIZE, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
    }
    else
    {
        for (int id = 1; id < num_proc; ++id)
        {
            if (id == 0)
            {
                start = 1;
            }
            else
            {
                start = id * (ISIZE / num_proc);
            }

            if (id == num_proc - 1)
            {
                finish = ISIZE - 1;
            }
            else
            {
                finish = start + (ISIZE / num_proc);
            }

            MPI_Recv(&a[start * JSIZE], (finish - start) * JSIZE, MPI_DOUBLE, id, 0, MPI_COMM_WORLD, &status);
        }
    }

    double finish_t = MPI_Wtime();

    MPI_Finalize();

    if (proc_id != 0)
    {
        free(a);
        return 0;
    }
    printf("%lf\n", finish_t-start_t);

    ff = fopen("result.txt","w");
    for(i = 0; i < ISIZE; i++)
    {
        for (j = 0; j < JSIZE; j++)
        {
            fprintf(ff, "%f ",a[i * JSIZE + j]);
        }
        fprintf(ff, "\n");
    }
    fclose(ff);
    free(a);

    return 0;
}
