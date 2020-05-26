// /**
//  * Name: Kim Seonggyeom
//  * Student id: 20668626
//  * ITSC email: skimbc@connect.ust.hk
// */

// #include "mpi_smith_waterman.h"

// /*
//  *  You can add helper functions and variables as you wish.
//  */
// #include <algorithm>
// #include <iostream>

// // #define DEBUG
// #ifdef DEBUG
// void print(int *&array, int n)
// {
//     for (int i = 0; i < n; i++)
//     {
//         char buffer[15];
//         sprintf(buffer, "%3d", array[i]);
//         cout << buffer << " ";
//         // cout << array[i] << " ";
//     }
//     cout << endl;
// }
// #endif
// void list(int *&array, int n)
// {
//     array = new int[n];

//     for (int i = 0; i < n; i++)
//     {
//         array[i] = 0;
//     }
// }
// int smith_waterman(int my_rank, int p, MPI_Comm comm, char *a, char *b, int a_len, int b_len)
// {
//     /*
//      *  Please fill in your codes here.
//      */
//     int max_score = 0;
//     int global_max_score = 0;
//     int comm_size = 0;
//     MPI_Comm_size(comm, &comm_size);

//     // param initialization
//     MPI_Bcast(&a_len, 1, MPI_INT, 0, comm);
//     MPI_Bcast(&b_len, 1, MPI_INT, 0, comm);

//     int amt_work = a_len / comm_size;
//     int remain_work = a_len % comm_size;
//     int start = 1;
//     if (my_rank < remain_work)
//     {
//         start += my_rank * amt_work + my_rank;
//         amt_work++;
//     }
//     else
//     {
//         start += my_rank * amt_work + remain_work;
//     }
//     int end = start + amt_work - 1;

//     if (my_rank != 0)
//     {
//         a = new char[a_len + 1];
//         b = new char[b_len + 1];
//     }
//     MPI_Bcast(a, a_len + 1, MPI_CHAR, 0, comm);
//     MPI_Bcast(b, b_len + 1, MPI_CHAR, 0, comm);

// #ifdef DEBUG
//     // printf("comm_size: %d, my rank: %d, a=%s, b=%s, a_len=%d, b_len=%d \n", comm_size,my_rank, a, b, a_len, b_len);
//     printf("comm_size: %d, my rank: %d, cnt=%d, start=%d, end=%d \n", comm_size, my_rank, amt_work, start, end);
// #endif
//     if (my_rank == 0)
//     { // root processor
//         int **score = new int *[amt_work + 1];
//         list(score[0], b_len + 1);
//         for (int j = 1; j <= b_len; j++)
//         {
//             int k = 1;
//             for (int i = start; i <= end; i++)
//             {
//                 if (j == 1)
//                     list(score[k], b_len + 1);
//                 score[k][j] = max(0,
//                                   max(score[k - 1][j - 1] + sub_mat(a[i - 1], b[j - 1]),
//                                       max(score[k - 1][j] - GAP,
//                                           score[k][j - 1] - GAP)));
//                 max_score = max(max_score, score[k][j]);
//                 k++;
//             }
//             if (comm_size > 1)
//             {
//                 MPI_Send(&score[k - 1][j], 1, MPI_INT, my_rank + 1, 0, comm);
//             }

//             // cout<<score[k-1][j];
//         }
//     }
//     else if (my_rank < comm_size - 1)
//     { // middle processors
//         int **score = new int *[amt_work + 1];
//         list(score[0], b_len + 1);
//         for (int j = 1; j <= b_len; j++)
//         {
//             int k = 1;
//             MPI_Recv(&score[k - 1][j], 1, MPI_INT, my_rank - 1, 0, comm, MPI_STATUS_IGNORE);
//             for (int i = start; i <= end; i++)
//             {
//                 if (j == 1)
//                     list(score[k], b_len + 1);
//                 score[k][j] = max(0,
//                                   max(score[k - 1][j - 1] + sub_mat(a[i - 1], b[j - 1]),
//                                       max(score[k - 1][j] - GAP,
//                                           score[k][j - 1] - GAP)));
//                 max_score = max(max_score, score[k][j]);
//                 k++;
//             }
//             MPI_Send(&score[k - 1][j], 1, MPI_INT, my_rank + 1, 0, comm);
//         }
//     }
//     else
//     { // last processor
//         int **score = new int *[amt_work + 1];
//         list(score[0], b_len + 1);
//         for (int j = 1; j <= b_len; j++)
//         {
//             int k = 1;
//             MPI_Recv(&score[k - 1][j], 1, MPI_INT, my_rank - 1, 0, comm, MPI_STATUS_IGNORE);
//             for (int i = start; i <= end; i++)
//             {
//                 if (j == 1)
//                     list(score[k], b_len + 1);
//                 score[k][j] = max(0,
//                                   max(score[k - 1][j - 1] + sub_mat(a[i - 1], b[j - 1]),
//                                       max(score[k - 1][j] - GAP,
//                                           score[k][j - 1] - GAP)));
//                 max_score = max(max_score, score[k][j]);
//                 k++;
//             }
//         }
//     }

//     MPI_Reduce(&max_score, &global_max_score, 1, MPI_INT, MPI_MAX, 0, comm);

//     return global_max_score;
// }
#include <iostream>
#include <string>
#include <algorithm>
#include <chrono>

#include "mpi_smith_waterman.h"

int smith_waterman(int my_rank, int p, MPI_Comm comm, char *a, char *b, int a_len, int b_len) {
    MPI_Bcast(&a_len, 1, MPI_INT, 0, comm);
    MPI_Bcast(&b_len, 1, MPI_INT, 0, comm);
    if (my_rank > 0) {
        a = (char *)malloc(sizeof(char) * (a_len + 1));
        b = (char *)malloc(sizeof(char) * (b_len + 1));
    }
    MPI_Bcast(a, a_len + 1, MPI_CHAR, 0, comm);
    MPI_Bcast(b, b_len + 1, MPI_CHAR, 0, comm);

    // block boundary: [l, r)
    int block_l = my_rank * b_len / p, block_r = (my_rank + 1) * b_len / p;
    int width = block_r - block_l;

    int **score = (int **)malloc(sizeof(int*) * (a_len + 1));
    for (int i = 0; i <= a_len; i++) {
        score[i] = (int*)calloc(width + 1, sizeof(int));
    }

    int n_diagonal = a_len + p - 1;
    int max_score = 0;
    for (int i = 0; i < n_diagonal; i++) {
        int row = i - my_rank + 1;
        if (row > 0 && row <= a_len) {
            for (int j = 1; j <= width; j++) {
                score[row][j] = max(0,
                                max(score[row - 1][j - 1] + sub_mat(a[row - 1], b[block_l + j - 1]),
                                max(score[row - 1][j] - GAP,
                                    score[row][j - 1] - GAP)));
                max_score = max(max_score, score[row][j]);
            }
            if (my_rank < p - 1) {
                MPI_Send(&score[row][width], 1, MPI_INT, my_rank + 1, 0, comm);
            }
        }
        if (my_rank > 0 && row + 1 > 0 && row + 1 <= a_len) {
            MPI_Recv(&score[row + 1][0], 1, MPI_INT, my_rank - 1, 0, comm, MPI_STATUS_IGNORE);
        }
    }

    int global_max = 0;
    MPI_Reduce(&max_score, &global_max, 1, MPI_INT, MPI_MAX, 0, comm);
    
    if (my_rank > 0) {
        free(a);
        free(b);
    }
    for (int i = 0; i <= a_len; i++) {
        free(score[i]);
    }
    free(score);
    
    return global_max;
}
