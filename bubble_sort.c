#include <stdio.h>
#include <stdlib.h>

#define ARRAY_SIZE 100000

#include <sys/time.h>

double get_time(void) {
    struct timeval tv;

    double t;

    if (gettimeofday(&tv, NULL) != 0) {
        perror("gettimeofday");
        exit(1);
    }
    t = (double) tv.tv_sec + (double) tv.tv_usec * 1.0e-6;

    return t;
}

/*
 * Buble sort algorithm.
 */
void bs(int n, int *vetor) {
    int c = 0, d, troca, trocou = 1;

    while ((c < (n - 1)) & trocou) {
        trocou = 0;
        for (d = 0; d < n - c - 1; d++)
            if (vetor[d] > vetor[d + 1]) {
                troca = vetor[d];
                vetor[d] = vetor[d + 1];
                vetor[d + 1] = troca;
                trocou = 1;
            }
        c++;
    }
}

int main() {
    int arr[ARRAY_SIZE];
    double t0, t1;
    int i;

    for(i = 0; i < ARRAY_SIZE; ++i)
        arr[i] = ARRAY_SIZE - i;


    t0 = get_time();
    bs(ARRAY_SIZE, arr);
    t1 = get_time();

    printf("Time %f\n", t1 - t0);

    return 0;
}

