BWUTILS_PATH = '/home/ybw/bwutils'

bcast_tracktime: $(BWUTILS_PATH)/main_bcast.cpp
	mpiicc -O2 -o bcast_tracktime main_bcast.cpp -L/home/ybw/libtrace -lmpit -lz
clean:
	rm bcast_tracktime
	rm bcast.*
	rm mpi_trace*
run:
	I_MPI_DEBUG=4 mpirun -n 64 ./bcast_tracktime
run16:
	I_MPI_DEBUG=4 mpirun -n 16 ./bcast_tracktime
