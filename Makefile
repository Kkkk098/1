all:
	mpicc -o res_mpi.out c-mpi/main.c
	@echo "Make MPI C"

	gcc -o res_pthreads.out p-threads/main.c
	@echo "Make PThreads C"

	gcc -o res_openmp.out -fopenmp open-mp/main.c
	@echo "Make OMP C"
