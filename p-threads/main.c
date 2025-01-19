#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>

struct thread_args
{
    int index_start;
    int index_end;
    int *result;
};

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

void *compute_prime_numbers(void *args)
{
    struct thread_args *st_args = (struct thread_args *)args;
    int local_count = st_args->index_end - st_args->index_start;

    for (int i = 0; i < local_count; i++)
    {
        st_args->result[i] = is_prime(st_args->index_start + i);
    }

    return NULL;
}

#define CHUNK_SIZE (1000000)
#define OUTPUT_FILE_NAME ("res_pthreads")

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

    int size_t = atoi(argv[1]);
    int num_to_check = atoi(argv[2]);
    struct thread_args *arguments = NULL;

    pthread_t *threads = NULL;
    threads = (pthread_t *)malloc(size_t * sizeof(pthread_t));
    arguments = (struct thread_args *)malloc(size_t * sizeof(struct thread_args));

    int chunk_size = num_to_check / size_t;
    int remainder = num_to_check % size_t;

    for (int i = 0; i < size_t; i++)
    {
        arguments[i].index_start = chunk_size * i + (i < remainder ? i : remainder);
        arguments[i].index_end = arguments[i].index_start + chunk_size + (i < remainder ? 1 : 0);
        arguments[i].result = (int *)malloc((arguments[i].index_end - arguments[i].index_start) * sizeof(int));

        pthread_create(&threads[i], NULL, compute_prime_numbers, &arguments[i]);
    }

    char char_buffer[CHUNK_SIZE];
    int char_buffer_counter = 0;

    for (int i = 0; i < size_t; i++)
    {
        pthread_join(threads[i], NULL);
        int local_count = arguments[i].index_end - arguments[i].index_start;

        for (int j = 0; j < local_count; j++)
        {
            if (arguments[i].result[j])
            {
                char_buffer_counter += sprintf(&char_buffer[char_buffer_counter], "%d \n", arguments[i].index_start + j);
            }
        }

        free(arguments[i].result);
    }

    double time_result = (double)(clock() - time_spent) / CLOCKS_PER_SEC;
    printf("C PThreads on %d numbers. Time: %f \n", num_to_check, time_result);

    output_result(char_buffer, char_buffer_counter);

    free(threads);
    free(arguments);

    return 0;
}