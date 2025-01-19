#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>

#define CHUNK_SIZE (1000000)
#define OUTPUT_FILE_NAME ("res_openmp")

void omp_set_num_threads(int) __GOMP_NOTHROW;

void output_result(char result_str[], int char_buffer_counter)
{
    FILE *file_ptr;

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
        fwrite(result_str, sizeof(char), char_buffer_counter, file_ptr);
        fclose(file_ptr);
    }
}

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

int compare(const void *a, const void *b)
{
    int int_a = *(int *)a;
    int int_b = *(int *)b;

    return (int_a > int_b) - (int_a < int_b);
}

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        fprintf(stderr, "USAGE: ./prime.out <Integer value>\n");
        exit(1);
    }

    if (atoi(argv[2]) < 2)
    {
        fprintf(stderr, "USAGE: %d must be >= 2\n", atoi(argv[1]));
        exit(1);
    }

    clock_t time_spent = clock();

    int i;
    int *prime_numbers = NULL;
    int size_t = atoi(argv[1]);
    int num_to_check = atoi(argv[2]);
    prime_numbers = (int *)malloc(num_to_check * sizeof(int));
    int count = 0;

    omp_set_num_threads(size_t);

#pragma omp parallel shared(prime_numbers)
    {
#pragma omp for
        for (i = 0; i < num_to_check; i++)
        {
            if (is_prime(i))
            {
#pragma omp critical
                {
                    prime_numbers[count++] = i;
                }
            }
        }
    }

    double time_result = (double)(clock() - time_spent) / CLOCKS_PER_SEC;
    printf("C OMP on %d numbers. Time: %f \n", num_to_check, time_result);

    qsort(prime_numbers, count, sizeof(int), compare);

    char char_buffer[CHUNK_SIZE];
    int char_buffer_counter = 0;

    for (int i = 0; i < count; i++)
    {
        char_buffer_counter += sprintf(&char_buffer[char_buffer_counter], "%d \n", prime_numbers[i]);
    }

    output_result(char_buffer, char_buffer_counter);

    free(prime_numbers);
}
