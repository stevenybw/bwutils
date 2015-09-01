/*
 * Utilities that record the communication traces of
 * bcast algorithms.
 *
 * input: none
 * output: bcast.$(PROCNUM).$(PROCID).log
 *
 * Bowen Yu <stevenybw@hotmail.com>
 * 2015.9.1
 */
#include <iostream>
#include <cstring>
#include <cstdio>
#include <sys/time.h>
#include <mpi.h>

using namespace std;

#define SIZE 		524288
#define MAX_NPROCS	100
#define MPIR_ALLTOALL_TAG		9
#define MPIR_BCAST_TAG          2

char sbuf[SIZE*MAX_NPROCS], rbuf[SIZE*MAX_NPROCS];
FILE* fp;

double currentTime()
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (double)tv.tv_sec*1000000.0 + tv.tv_usec;
}

int MY_BINOMIAL_Bcast(char* buffer, int count, MPI_Datatype datatype, int root, MPI_Comm comm)
{
    int          comm_size, i;
    MPI_Aint     sendtype_extent, recvtype_extent;
    int mpi_errno=MPI_SUCCESS;
    int mpi_errno_ret = MPI_SUCCESS;
    int src, dst, rank;
    double begin, end;
    MPI_Status status;

    MPI_Comm_size(comm, &comm_size);
    MPI_Comm_rank(comm, &rank);

    int relative_rank = (rank >= root) ? rank - root : rank - root + comm_size;

    int mask = 0x1;
	double bt, et;

    double bcast_begin_time = currentTime();

    while (mask < comm_size) {
        if (relative_rank & mask) {
            bt = currentTime();
            src = rank - mask;
            if (src < 0)
                src += comm_size;
            mpi_errno = MPI_Recv(buffer, count, datatype, src,
                                     MPIR_BCAST_TAG, comm, &status);

            et = currentTime();
            fprintf(fp, "(%0.0lf) RECV[src=%d]> %0.0lf elapsed\n", bt-bcast_begin_time, src, et-bt);
            break;
        }
        mask <<= 1;
    }

    mask >>= 1;
    while (mask > 0) {
	    bt = currentTime();
        if (relative_rank + mask < comm_size) {
            dst = rank + mask;
            if (dst >= comm_size)
                dst -= comm_size;
            mpi_errno = MPI_Send(buffer, count, datatype, dst,
                                     MPIR_BCAST_TAG, comm);
            et = currentTime();
            fprintf(fp, "(%0.0lf) SEND[dst=%d]> %0.0lf elapsed\n", bt-bcast_begin_time, dst, et-bt);
        }
        mask >>= 1;
    }

    return (mpi_errno);
}

int main(int argc, char* argv[])
{
	int rank, size;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	char buf[256];
	sprintf(buf, "bcast.%d.%d.log", size, rank);

	fp = fopen(buf, "w");

	memset(sbuf, 1, sizeof(sbuf));
	memset(rbuf, 1, sizeof(rbuf));

	printf("Executing all to all...\n");

	for(int i=0;i<10;i++)
	{
		MPI_Barrier(MPI_COMM_WORLD);
		double be=currentTime();
		fprintf(fp, "%lf MY_BINOMIAL_Bcast ENTER\n", be);
		MY_BINOMIAL_Bcast(sbuf, SIZE, MPI_CHAR, 0, MPI_COMM_WORLD);
		double ed=currentTime();
		fprintf(fp, "%lf MY_BINOMIAL_Bcast LEAVING, %lf elapsed\n", ed, ed-be);
		MPI_Barrier(MPI_COMM_WORLD);
	}

	fclose(fp);

	MPI_Finalize();

	return 0;
}
