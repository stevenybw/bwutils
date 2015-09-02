BWUTILS_PATH = '/home/ybw/bwutils'
TIMESTAMP = $(shell date +%Y%m%d_%H.%M.%S)

bcast_trace.out: main_bcast.cpp
	mpiicc -O2 -o bcast_tracktime.out main_bcast.cpp -L/home/ybw/libtrace #-lmpit -lz
clean:
	rm *.out
run:
	mkdir BCAST_RESULT_$(TIMESTAMP)
	cd BCAST_RESULT_$(TIMESTAMP) && I_MPI_DEBUG=4 mpirun -n 64 ../bcast_tracktime.out && ../visualize_comm_trace.py 64 | dot -Tpng -oBCAST_RESULT_$(TIMESTAMP).png 
run16:
	I_MPI_DEBUG=4 mpirun -n 16 ./bcast_tracktime.out
