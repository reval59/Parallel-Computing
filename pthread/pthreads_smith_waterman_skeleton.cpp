/**
 * Name: Kim Seonggyeom 
 * Student id: 20668626
 * ITSC email: skimbc@connect.ust.hk
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <algorithm>
#include "pthreads_smith_waterman.h"

/*
 *  You can add helper functions and variables as you wish.
 */

/* Global variable:  accessible to all threads */
int *local_max;
int thread_count;
char *a_global;
char *b_global;
int a_len_global;
int b_len_global;
sem_t *send_semaphores;
// sem_t* recv_semaphores;
int **scores_to_pass;

// #define DEBUG
void *thread_smith_waterman(void *rank)
{
    long my_rank = (long)rank; /* Use long in case of 64-bit system */

    int start = my_rank * b_len_global / thread_count;
    int col = ((my_rank + 1) * b_len_global / thread_count) - start;
    int **score = (int **)malloc(sizeof(int *) * (a_len_global + 1));
    for (int i = 0; i <= a_len_global; i++)
    {
        score[i] = (int *)calloc(col + 1, sizeof(int));
    }

    if (my_rank < thread_count - 1)
    {
        scores_to_pass[my_rank] = (int *)calloc(a_len_global, sizeof(int));
    }
    int num_iteration = a_len_global + thread_count - 1;

#ifdef DEBUG
    printf("thread: %ld, start: %d, col: %d, num_iteration: %d \n", my_rank, start, col, num_iteration);
#endif
    int max_score = 0, k = 0, l = 0;
    for (int i = 0; i < num_iteration; i++)
    {
        int row = i - my_rank + 1;
        if (row > 0 && row <= a_len_global)
        {
            for (int j = 1; j <= col; j++)
            {
                score[row][j] = max(0,
                                    max(score[row - 1][j - 1] + sub_mat(a_global[row - 1], b_global[start + j - 1]),
                                        max(score[row - 1][j] - GAP,
                                            score[row][j - 1] - GAP)));
                max_score = max(max_score, score[row][j]);
            }
            if (my_rank < thread_count - 1)
            {

                scores_to_pass[my_rank][k] = score[row][col];
                k++;
                sem_post(&send_semaphores[my_rank + 1]);
            }
        }
        if (my_rank > 0 && row + 1 > 0 && row + 1 <= a_len_global)
        {

            sem_wait(&send_semaphores[my_rank]);
            score[row + 1][0] = scores_to_pass[my_rank - 1][l];
            l++;
        }
    }

    for (int i = 0; i <= a_len_global; i++)
    {
        free(score[i]);
    }
    free(score);

    local_max[my_rank] = max_score;
    return NULL;
}

int smith_waterman(int num_threads, char *a, char *b, int a_len, int b_len)
{
    /*
     *  Please fill in your codes here.
     */
    long thread; /* Use long in case of a 64-bit system */
    pthread_t *thread_handles;
    int global_max = 0;
    thread_count = num_threads;
    a_global = a;
    b_global = b;
    a_len_global = a_len;
    b_len_global = b_len;

#ifdef DEBUG
    printf("num_threads: %d, a=%s, b=%s, a_len=%d, b_len=%d \n", num_threads, a_global, b_global, a_len_global, b_len_global);
#endif

    thread_handles = (pthread_t *)malloc(num_threads * sizeof(pthread_t));
    local_max = (int *)calloc(num_threads, sizeof(int));

    send_semaphores = (sem_t *)malloc(num_threads * sizeof(sem_t));
    // recv_semaphores = (sem_t*) malloc(num_threads*sizeof(sem_t));

    scores_to_pass = (int **)malloc(sizeof(int *) * (num_threads - 1));

    for (thread = 0; thread < num_threads; thread++)
    {
        sem_init(&send_semaphores[thread], 0, 0);
        //   sem_init(&recv_semaphores[thread], 0, 0);
    }

    for (thread = 0; thread < num_threads; thread++)
    {
        pthread_create(&thread_handles[thread], NULL, thread_smith_waterman, (void *)thread);
    }

    for (thread = 0; thread < num_threads; thread++)
    {
        pthread_join(thread_handles[thread], NULL);
        if (local_max[thread] > global_max)
        {
            global_max = local_max[thread];
        }
#ifdef DEBUG
        printf("local_max: %d\n", local_max[thread]);
#endif
    }
    free(thread_handles);

    return global_max;
}
