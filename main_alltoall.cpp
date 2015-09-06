#include <iostream>
#include <cstring>
#include <cstdio>
#include <sys/time.h>
#include <mpi.h>

using namespace std;

#define SIZE         524288
#define MAX_NPROCS    100
#define MPIR_ALLTOALL_TAG        9
#define MPIR_BCAST_TAG          2

char sbuf[SIZE*MAX_NPROCS], rbuf[SIZE*MAX_NPROCS];
FILE* fp;

bool enabled_trace = false;
double bt, et;
double init_time;

#define _TRACE(stmt) do{if(enabled_trace) stmt;} while(0)
#define DISABLE_TRACE do{enabled_trace=0;}while(0)
#define ENABLE_TRACE do{enabled_trace=1;}while(0)

// for example ENTER_ITEM("MPI_BCAST")
#define ENTER_ITEM(item_name) do{init_time=currentTime(); fprintf(fp, "ENTER_ITEM %s %0.0lf\n", item_name, init_time); } while(0)

double currentTime()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (double)tv.tv_sec*1000000.0 + tv.tv_usec;
}

int MY_PAIRWISE_Alltoall(const char* sendbuf, int sendcount, MPI_Datatype sendtype, char* recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm)
{
    int          comm_size, i, pof2;
    MPI_Aint     sendtype_extent, recvtype_extent;
    int mpi_errno=MPI_SUCCESS;
    int mpi_errno_ret = MPI_SUCCESS;
    int src, dst, rank;
    double begin, end;
    MPI_Status status;

    MPI_Comm_size(comm, &comm_size);
    MPI_Comm_rank(comm, &rank);


    /* Get extent of send and recv types */
    MPI_Type_extent(sendtype, &sendtype_extent);
    MPI_Type_extent(recvtype, &recvtype_extent);
    
    /* Is comm_size a power-of-two? */
    i = 1;
    while (i < comm_size)
        i *= 2;
    
        if (i == comm_size) {
        pof2 = 1;
    } else  {
        pof2 = 0;
    }
    
    /* Do the pairwise exchanges */
    for (i=1; i<comm_size; i++) {
        bt=currentTime();
        if (pof2 == 1) {
            /* use exclusive-or algorithm */
            src = dst = rank ^ i;
        } else {
            src = (rank - i + comm_size) % comm_size;
            dst = (rank + i) % comm_size;
        }
        mpi_errno = MPI_Sendrecv(((char *)sendbuf +
                                     dst*sendcount*sendtype_extent),
                                     sendcount, sendtype, dst,
                                     MPIR_ALLTOALL_TAG,
                                     ((char *)recvbuf +
                                     src*recvcount*recvtype_extent),
                                     recvcount, recvtype, src,
                                     MPIR_ALLTOALL_TAG, comm, &status);
        et=currentTime();
        _TRACE(fprintf(fp, "%0.0lf %0.0lf MPI_SENDRECV %d %d\n", bt-init_time, et-bt, src, dst));

        if(mpi_errno)
            printf("Error!\n");
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
    sprintf(buf, "alltoall.%d.%d.log", size, rank);
    fp = fopen(buf, "w");

    memset(sbuf, 1, sizeof(sbuf));
    memset(rbuf, 1, sizeof(rbuf));

    printf("Testing experiment ALLTOALL...\n");

    ENABLE_TRACE;

    for(int i=0;i<10;i++)
    {
        MPI_Barrier(MPI_COMM_WORLD);
        sprintf(buf, "MY_PAIRWISE_Alltoall_%d", i);
        ENTER_ITEM(buf);
        MY_PAIRWISE_Alltoall(sbuf, SIZE, MPI_CHAR, rbuf, SIZE, MPI_CHAR, MPI_COMM_WORLD);
        MPI_Barrier(MPI_COMM_WORLD);
    }

    DISABLE_TRACE;

    fclose(fp);
    MPI_Finalize();

    return 0;
}
