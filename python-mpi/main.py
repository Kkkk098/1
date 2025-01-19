from mpi4py import MPI
import sys
import math
import time

def is_prime(n):
    if n <= 1:
        return False
    if n <= 3:
        return True
    if n % 2 == 0 or n % 3 == 0:
        return False
    for i in range(5, int(math.sqrt(n)) + 1, 6):
        if n % i == 0 or n % (i + 2) == 0:
            return False
    return True

def calculate_primes(start, end):
    return [n for n in range(start, end) if is_prime(n)]

def main():
    start_time = time.time()

    comm = MPI.COMM_WORLD
    rank = comm.Get_rank() 
    size = comm.Get_size()       

    if len(sys.argv) != 2:
        if rank == 0:
            print("Usage: python3 primes_mpi.py <number_of_threads> <upper_limit>")
        sys.exit(1)

    upper_limit = int(sys.argv[1])

    chunk_size = upper_limit // size
    start = rank * chunk_size + (1 if rank == 0 else 0)
    end = start + chunk_size if rank != size - 1 else upper_limit + 1

    local_primes = calculate_primes(start, end)

    all_primes = comm.gather(local_primes, root=0)

    if rank == 0:
        print("Python MPI on %d numbers. Time: %s" % (upper_limit, time.time() - start_time))
        all_primes = [p for sublist in all_primes for p in sublist]
        all_primes.sort()

        write_string_buffer = [];
        
        for prime in all_primes:
            write_string_buffer.append(str(prime) + "\n");
        
        try:
            with open("./results/res_python_mpi.txt", "w") as f:
                f.write("".join(write_string_buffer))
        except FileExistsError:
            print("Already exists.")


if __name__ == "__main__":
    main()