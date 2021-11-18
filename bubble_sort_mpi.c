#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"

#define DEBUG 0
#define ARRAY_SIZE 100000

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

/* Recebe um ponteiro para um vetor que contem as mensagens recebidas dos filhos e
 * intercala estes valores em um terceiro vetor auxiliar. Devolve um ponteiro para este vetor
 */
int* interleaving(int vetor[], int tam) {
    int *vetor_auxiliar;
    int i1, i2, i_aux;

    vetor_auxiliar = (int*) malloc(sizeof(int) * tam);

    i1 = 0;
    i2 = tam / 2;

    for (i_aux = 0; i_aux < tam; i_aux++) {
        if (((vetor[i1] <= vetor[i2]) && (i1 < (tam / 2))) || (i2 == tam))
            vetor_auxiliar[i_aux] = vetor[i1++];
        else
            vetor_auxiliar[i_aux] = vetor[i2++];
    }

    return vetor_auxiliar;
}

int main(int argc, char** argv) {
    int proc_n;
    int my_rank;
    int tag = 123;
    int vector_size;
    int i;
    double t0, t1;
    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &proc_n);

    int arr[ARRAY_SIZE];
    int *temp;

    if (my_rank == 0) {
        vector_size = ARRAY_SIZE;

        for (i = 0; i < vector_size; ++i)
            arr[i] = vector_size - i;
        t0 = MPI_Wtime();

    } else {

        MPI_Recv(&arr, ARRAY_SIZE, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        MPI_Get_count(&status, MPI_INT, &vector_size);
    }

    int delta = ARRAY_SIZE / ((proc_n + 1) / 2); // Dividir por dois ja que sou arvore binaria
    int parent = (my_rank - 1) / 2;

    if (vector_size <= delta) {

        printf("Conquisto! rank: %d tamanho vetor: %d\n", my_rank, vector_size);
        bs(vector_size, arr);
        MPI_Send(arr, vector_size, MPI_INT, parent, tag, MPI_COMM_WORLD);

    } else {
        int new_size = vector_size / 2;
        int left = 2 * my_rank + 1;
        int right = 2 * my_rank + 2;

        printf("Divido! rank: %d left: %d right: %d\n", my_rank, left, right);

        // Mando para o filho da esquerda
        MPI_Send(&arr[0], new_size, MPI_INT, left, tag, MPI_COMM_WORLD);
        // Mando para o filho da direita
        MPI_Send(&arr[new_size], new_size, MPI_INT, right, tag, MPI_COMM_WORLD);


        // recebo da esquerda
        MPI_Recv(&arr[0], new_size, MPI_INT, left, tag, MPI_COMM_WORLD, &status);
        // recebo da direita
        MPI_Recv(&arr[new_size], new_size, MPI_INT, right, tag, MPI_COMM_WORLD, &status);
    }

    temp = interleaving(arr, vector_size);

    if (my_rank != 0) {
        MPI_Send(temp, vector_size, MPI_INT, parent, tag, MPI_COMM_WORLD);

    } else if(my_rank == 0){
        temp = interleaving(temp, vector_size);
        t1 = MPI_Wtime();

        printf("Time %f\n", t1 - t0);

#if DEBUG
        for (i = 0; i < vector_size; ++i)
            printf("[0%d] ", temp[i]);
#endif
    }

    free(temp);
    MPI_Finalize();
    return 0;
}
