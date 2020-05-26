/**
 * Name: Kim Seonggyeom 
 * Student id: 20668626
 * ITSC email: skimbc@connect.ust.hk
*/

#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <chrono>
#include <cassert>
#include <stdio.h>

#include "cuda_smith_waterman.h"

using namespace std;

__global__ void init_score_matrix(int *d_a_len, int *d_b_len, int *d_score)
{
    int threadID = blockDim.x * blockIdx.x + threadIdx.x;
    int numThreads = gridDim.x * blockDim.x;

    int atdia_len = min(*d_a_len, *d_b_len) + 2;
    int total_len = 3 * atdia_len;

    for (int i = threadID; i < total_len; i += numThreads)
    {
        d_score[i] = 0;
    }
}
__global__ void init_max_scores(int *d_max_scores){
    int threadID = blockDim.x * blockIdx.x + threadIdx.x;
    d_max_scores[threadID] = 0;
}
__global__ void smith_waterman_per_row(int first_atdia_idx, int second_atdia_idx, int current_atdia_idx, int cnt, int i, int j, char *d_a, char *d_b, int *d_score, int *d_max_scores)
{
    int threadID = blockDim.x * blockIdx.x + threadIdx.x;
    int numThreads = gridDim.x * blockDim.x;

    for (int l = threadID + 1; l <= cnt; l += numThreads)
    {
        d_score[current_atdia_idx + l] = max(0,
                                             max(d_score[first_atdia_idx + l - 1] + sub_mat(d_a[i - (l - 1) - 1], d_b[j + (l - 1) - 1]),
                                                 max(d_score[second_atdia_idx + l - 1] - GAP,
                                                     d_score[second_atdia_idx + l] - GAP)));

        d_max_scores[threadID] = max(d_max_scores[threadID], d_score[current_atdia_idx + l]);
    }
}
int smith_waterman(int blocks_per_grid, int threads_per_block, char *a, char *b, int a_len, int b_len)
{
    int max_score = 0;
    int *max_scores = (int *)malloc(sizeof(int) * (threads_per_block * blocks_per_grid));

    dim3 blocks(blocks_per_grid);
    dim3 threads(threads_per_block);

    int num_iter = a_len + b_len - 1;
    int atdia_len = min(a_len, b_len) + 2; // anti diagonal length

    int first_atdia_idx = 0; // first anti diagonal index
    int second_atdia_idx = atdia_len; // second anti diagonal index

    char *d_a;
    char *d_b;
    int *d_a_len;
    int *d_b_len;
    int *d_score;
    int *d_max_scores; // local max scores per thread

    cudaMalloc(&d_a, sizeof(char) * (a_len + 1));
    cudaMalloc(&d_b, sizeof(char) * (b_len + 1));
    cudaMalloc(&d_a_len, sizeof(int));
    cudaMalloc(&d_b_len, sizeof(int));
    cudaMalloc(&d_score, sizeof(int) * (3 * atdia_len));
    cudaMalloc(&d_max_scores, sizeof(int) * (threads_per_block * blocks_per_grid));

    cudaMemcpy(d_a, a, sizeof(char) * (a_len + 1), cudaMemcpyHostToDevice);
    cudaMemcpy(d_b, b, sizeof(char) * (b_len + 1), cudaMemcpyHostToDevice);
    cudaMemcpy(d_a_len, &a_len, sizeof(int), cudaMemcpyHostToDevice);
    cudaMemcpy(d_b_len, &b_len, sizeof(int), cudaMemcpyHostToDevice);

    init_score_matrix<<<blocks, threads>>>(d_a_len, d_b_len, d_score);
    cudaDeviceSynchronize();

    init_max_scores<<<blocks, threads>>>(d_max_scores);
    cudaDeviceSynchronize();

    // travese each anti diagonal row from top left to bottom right
    for (int k = 1; k <= num_iter; k++)
    {
        int i = min(k, a_len);
        int j = max(0, k - a_len) + 1;
        int cnt = min(k, min(b_len - j + 1, i));
        if (j > 1)
        {
            first_atdia_idx++;
            second_atdia_idx++;
        }
        int current_atdia_idx = ((k+1) % 3) * atdia_len;
        
        // process each anti diagonal row using GPU
        smith_waterman_per_row<<<blocks, threads>>>(first_atdia_idx, second_atdia_idx, current_atdia_idx, cnt, i, j, d_a, d_b, d_score, d_max_scores);
        cudaDeviceSynchronize();

        // change index
        first_atdia_idx = second_atdia_idx; 
        second_atdia_idx = current_atdia_idx; 
    }
    cudaMemcpy(max_scores, d_max_scores, sizeof(int) * (threads_per_block * blocks_per_grid), cudaMemcpyDeviceToHost);
    cudaDeviceSynchronize();
    for (int i = 0; i < threads_per_block * blocks_per_grid; i++)
    {
        max_score = max(max_score, max_scores[i]);
    }
    free(max_scores);

    return max_score;
}