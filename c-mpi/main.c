#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>

int is_prime(int n)
{
    if (n <= 1)
        return 0;
    if (n <= 3)
        return 1;
    if (n % 2 == 0 || n % 3 == 0)
        return 0;

    for (int i = 5; i * i <= n; i += 6)
    {
        if (n % i == 0 || n % (i + 2) == 0)
            return 0;
    }
    return 1;
}

#define CHUNK_SIZE (1000000)
#define OUTPUT_FILE_NAME ("res_mpi")

int main(int argc, char *argv[])
{
    clock_t time_spent = clock();

    int id, size;
    int num_to_check, start, end;
    int *results = NULL;
    int *local_results = NULL;
    FILE *file_ptr;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &id);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (argc != 2)
    {
        if (id == 0)
            printf("Usage: %s <number of numbers to calculate>\n", argv[0]);
        MPI_Finalize();
        return 1;
    }

    num_to_check = atoi(argv[1]);

    int chunk_size = num_to_check / size;
    int remainder = num_to_check % size;

    start = id * chunk_size + (id < remainder ? id : remainder);
    end = start + chunk_size + (id < remainder ? 1 : 0);

    int local_count = end - start;
    local_results = (int *)malloc(local_count * sizeof(int));

    for (int i = 0; i < local_count; i++)
    {
        local_results[i] = is_prime(start + i);
    }

    if (id == 0)
    {
        results = (int *)malloc(num_to_check * sizeof(int));
    }

    int *recvcounts = NULL;
    int *displs = NULL;

    if (id == 0)
    {
        recvcounts = (int *)malloc(size * sizeof(int));
        displs = (int *)malloc(size * sizeof(int));

        for (int i = 0; i < size; i++)
        {
            recvcounts[i] = (num_to_check / size) + (i < remainder ? 1 : 0);
            displs[i] = i * chunk_size + (i < remainder ? i : remainder);
        }
    }

    MPI_Gatherv(local_results, local_count, MPI_INT, results, recvcounts, displs, MPI_INT, 0, MPI_COMM_WORLD);

    // Root process outputs the primes
    if (id == 0)
    {
        double time_result = (double)(clock() - time_spent) / CLOCKS_PER_SEC;
        printf("C MPI on %d numbers. Time: %f \n", num_to_check, time_result);

        struct stat st = {0};
        if (stat("./results", &st) == -1)
        {
            mkdir("./results", 0700);
        }

        char file_name[100];
        sprintf(file_name, "./results/%s.txt", OUTPUT_FILE_NAME);

        file_ptr = fopen(file_name, "wb");
        if (file_ptr == NULL)
        {
            printf("Error opening file \n");
        }
        else
        {
            char char_buffer[CHUNK_SIZE];
            int char_buffer_counter = 0;

            for (int i = 0; i < num_to_check; i++)
            {
                if (results[i])
                {
                    char_buffer_counter += sprintf(&char_buffer[char_buffer_counter], "%d \n", i);
                }
            }

            fwrite(char_buffer, sizeof(char), char_buffer_counter, file_ptr);
            fclose(file_ptr);
        }

        free(results);
        free(recvcounts);
        free(displs);
    }

    free(local_results);

    MPI_Finalize();

    return 0;
}