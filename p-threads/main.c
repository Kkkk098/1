#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

struct thread_args {
    int     index_start;
    int     index_end;
    int     *result;
};

int is_prime(int n) {
    if (n <= 1) return 0;
    if (n <= 3) return 1;
    if (n % 2 == 0 || n % 3 == 0) return 0;

    for (int i = 5; i * i <= n; i += 6) {
        if (n % i == 0 || n % (i + 2) == 0) return 0;
    }
    return 1;
}

void* compute_prime_numbers(void* args) 
{
    struct thread_args *st_args = (struct thread_args *)args;
    int local_count = st_args->index_end - st_args->index_start;

    for (int i = 0; i < local_count; i++) {
        st_args->result[i] = is_prime(st_args->index_start + i);
    }

    return NULL;
}

int main(int argc, char *argv[]) {
   if (argc < 3) {
      fprintf(stderr, "USAGE: ./prime.out <Integer value>\n");
      exit(1);
   }

   if (atoi(argv[2]) < 2) {
      fprintf(stderr, "USAGE: %d must be >= 2\n", atoi(argv[1]));
      exit(1);
   }

    int size_t = atoi(argv[1]);
    int num_to_check = atoi(argv[2]);
    struct thread_args *arguments = NULL;
    
    pthread_t *threads = NULL;
    threads = (pthread_t *)malloc(size_t * sizeof(pthread_t));
    arguments = (struct thread_args *)malloc(size_t * sizeof(struct thread_args));

    int chunk_size = num_to_check / size_t;
    int remainder = num_to_check % size_t;

    for (int i = 0; i < size_t; i++) {
        arguments[i].index_start = chunk_size * i + (i < remainder ? i : remainder);
        arguments[i].index_end = arguments[i].index_start + chunk_size + (i < remainder ? 1 : 0);
        arguments[i].result = (int *)malloc((arguments[i].index_end - arguments[i].index_start) * sizeof(int));

        pthread_create(&threads[i], NULL, compute_prime_numbers, &arguments[i]);
    }

    for (int i = 0; i < size_t; i++) {
        pthread_join(threads[i], NULL);
        int local_count = arguments[i].index_end - arguments[i].index_start;

        for (int j = 0; j < local_count; j++) {
            if (arguments[i].result[j]) {
                printf("%d ", arguments[i].index_start + j);
            }
        }

        free(arguments[i].result);
    }

    free(threads);
    free(arguments);

    return 0;
}