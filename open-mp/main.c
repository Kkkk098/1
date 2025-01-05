#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

int is_prime(int n) {
    if (n <= 1) return 0;
    if (n <= 3) return 1;
    if (n % 2 == 0 || n % 3 == 0) return 0;

    for (int i = 5; i * i <= n; i += 6) {
        if (n % i == 0 || n % (i + 2) == 0) return 0;
    }
    return 1;
}

int compare(const void *a, const void *b) {
    int int_a = *(int*)a;
    int int_b = *(int*)b;

    return (int_a > int_b) - (int_a < int_b);
}

int main(int argc, char* argv[])
{
    if (argc < 3) {
        fprintf(stderr, "USAGE: ./prime.out <Integer value>\n");
        exit(1);
    }

    if (atoi(argv[2]) < 2) {
        fprintf(stderr, "USAGE: %d must be >= 2\n", atoi(argv[1]));
        exit(1);
    }

    int i;
    int *prime_numbers = NULL;
    int size_t = atoi(argv[1]);
    int num_to_check = atoi(argv[2]);
    prime_numbers = (int *)malloc(num_to_check* sizeof(int)); 
    int count = 0;

    omp_set_num_threads(size_t);

    #pragma omp parallel shared(prime_numbers)
    {
        #pragma omp for 
        for (i = 0; i < num_to_check; i++) {
            if (is_prime(i)) {
                #pragma omp critical
                {
                   prime_numbers[count++] = i;
                }
            }
        }
    }

    qsort(prime_numbers, count, sizeof(int), compare);
    for (int i = 0; i < count; i++) {
        printf("%d ", prime_numbers[i]);
    }

    printf("\n End calculating...\n");

    free(prime_numbers);
}
