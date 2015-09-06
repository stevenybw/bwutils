BWUTILS_PATH = '/home/ybw/bwutils'
TIMESTAMP = $(shell date +%Y%m%d_%H.%M.%S)

all: bcast_tracktime.out alltoall_tracktime.out

bcast_tracktime.out: main_bcast.cpp
	mpiicc -O2 -o bcast_tracktime.out main_bcast.cpp -L/home/ybw/libtrace #-lmpit -lz
alltoall_tracktime.out: main_alltoall.cpp
	mpiicc -O2 -o alltoall_tracktime.out main_alltoall.cpp -L/home/ybw/libtrace #-lmpit -lz
clean:
	rm *.out
run_bcast64:
	mkdir BCAST_RESULT_$(TIMESTAMP)
	cd BCAST_RESULT_$(TIMESTAMP) && I_MPI_DEBUG=4 mpirun -n 64 ../bcast_tracktime.out && ../visualize_comm_trace.py -n 64 -ppn 16 -t bcast  | dot -Tpng -oBCAST_RESULT_$(TIMESTAMP).png
run_alltoall64:
	mkdir ALLTOALL_RESULT_$(TIMESTAMP)
	cd ALLTOALL_RESULT_$(TIMESTAMP) && I_MPI_DEBUG=4 mpirun -n 64 ../alltoall_tracktime.out && ../visualize_comm_trace.py -n 64 -ppn 16 -t alltoall | dot -Tpng -O *.dot 
run_alltoall72:
	mkdir ALLTOALL_RESULT_$(TIMESTAMP)
	cd ALLTOALL_RESULT_$(TIMESTAMP) && I_MPI_DEBUG=4 mpirun -n 72 ../alltoall_tracktime.out && ../visualize_comm_trace.py -n 72 -ppn 18 -t alltoall | dot -Tpng -O *.dot 
run_bcast16:
	mkdir BCAST_RESULT_$(TIMESTAMP)
	cd BCAST_RESULT_$(TIMESTAMP) && I_MPI_DEBUG=4 mpirun -n 16 ../bcast_tracktime.out && ../visualize_comm_trace.py -n 16 -ppn 16 -t bcast  | dot -Tpng -oBCAST_RESULT_$(TIMESTAMP).png
