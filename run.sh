#!/bin/bash
module purge
module add compiler/gcc/11.2.0
module add mpi/openmpi/4.1.3/gcc/11
module load mpi/impi/5.1.3.210
module add python/3.9

make
mpiexec -n $1 ./res_mpi.out $2
./res_openmp.out $1 $2
./res_pthreads.out $1 $2
mpiexec -n $1 python3 ./python-mpi/main.py $2