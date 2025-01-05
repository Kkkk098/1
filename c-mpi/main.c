#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int is_prime(int n) {
    if (n <= 1) return 0;
    if (n <= 3) return 1;
    if (n % 2 == 0 || n % 3 == 0) return 0;

    for (int i = 5; i * i <= n; i += 6) {
        if (n % i == 0 || n % (i + 2) == 0) return 0;
    }
    return 1;
}

int main(int argc, char *argv[]) {
    int id, size;
    int num_to_check, start, end;
    int *results = NULL; 
    int *local_results = NULL;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &id);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (argc != 2) {
        if (id == 0) printf("Usage: %s <number of numbers to calculate>\n", argv[0]);
        MPI_Finalize();
        return 1;
    }

    num_to_check = atoi(argv[1]);
    if (id == 0) {
        printf("Calculating primes up to %d using %d processes...\n", num_to_check, size);
    }

    int chunk_size = num_to_check / size;
    int remainder = num_to_check % size;

    start = id * chunk_size + (id < remainder ? id : remainder);
    end = start + chunk_size + (id < remainder ? 1 : 0);

    int local_count = end - start;
    local_results = (int *)malloc(local_count * sizeof(int));

    for (int i = 0; i < local_count; i++) {
        local_results[i] = is_prime(start + i);
    }

    if (id == 0) {
        results = (int *)malloc(num_to_check * sizeof(int));
    }

    int *recvcounts = NULL;
    int *displs = NULL;

    if (id == 0) {
        recvcounts = (int *)malloc(size * sizeof(int));
        displs = (int *)malloc(size * sizeof(int));

        for (int i = 0; i < size; i++) {
            recvcounts[i] = (num_to_check / size) + (i < remainder ? 1 : 0);
            displs[i] = i * chunk_size + (i < remainder ? i : remainder);
        }
    }

    MPI_Gatherv(local_results, local_count, MPI_INT, results, recvcounts, displs, MPI_INT, 0, MPI_COMM_WORLD);

    // Root process outputs the primes
    if (id == 0) {
        printf("Prime numbers up to %d:\n", num_to_check);
        for (int i = 0; i < num_to_check; i++) {
            if (results[i]) {
                printf("%d ", i);
            }
        }
        printf("\n");

        free(results);
        free(recvcounts);
        free(displs);
    }

    free(local_results);
    
    MPI_Finalize();

    return 0;
}