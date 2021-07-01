/* 
 * trans.c - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 */
#include "cachelab.h"
#include <stdio.h>

int is_transpose(int M, int N, int A[N][M], int B[M][N]);

/* 
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded. 
 */
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N])
{
    int x, y, i;
    int v1, v2, v3, v4, v5, v6, v7, v8;
    for (x = 0; x < 32; x += 8)
        for (y = 0; y < 32; y += 8)
            for (i = y; i < y + 8; i++)
            {
                v1 = A[i][x];
                v2 = A[i][x + 1];
                v3 = A[i][x + 2];
                v4 = A[i][x + 3];
                v5 = A[i][x + 4];
                v6 = A[i][x + 5];
                v7 = A[i][x + 6];
                v8 = A[i][x + 7];
                B[x][i] = v1;
                B[x + 1][i] = v2;
                B[x + 2][i] = v3;
                B[x + 3][i] = v4;
                B[x + 4][i] = v5;
                B[x + 5][i] = v6;
                B[x + 6][i] = v7;
                B[x + 7][i] = v8;
            }
}

/* 
 * You can define additional transpose functions below. We've defined
 * a simple one below to help you get started. 
 */

/* 
 * trans - A simple baseline transpose function, not optimized for the cache.
 */
/*char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, tmp;

    for (i = 0; i < N; i++)
    {
        for (j = 0; j < M; j++)
        {
            tmp = A[i][j];
            B[j][i] = tmp;
        }
    }
}*/
/*
void Trans1(int M, int N, int A[N][M], int B[M][N])
{
    int x, y, i;
    int v1, v2, v3, v4, v5, v6, v7, v8;
    for (x = 0; x < 32; x += 8)
        for (y = 0; y < 32; y += 8)
            for (i = y; i < y + 8; i++)
            {
                v1 = A[i][x];
                v2 = A[i + 1][x];
                v3 = A[i + 2][x];
                v4 = A[i + 3][x];
                v5 = A[i + 4][x];
                v6 = A[i + 5][x];
                v7 = A[i + 6][x];
                v8 = A[i + 7][x];
                B[i][x] = v1;
                B[i][x + 1] = v2;
                B[i][x + 2] = v3;
                B[i][x + 3] = v4;
                B[i][x + 4] = v5;
                B[i][x + 5] = v6;
                B[i][x + 6] = v7;
                B[i][x + 7] = v8;
            }
}*/
/*
 * registerFunctions - This function registers your transpose
 *     functions with the driver.  At runtime, the driver will
 *     evaluate each of the registered functions and summarize their
 *     performance. This is a handy way to experiment with different
 *     transpose strategies.
 */
void registerFunctions()
{
    /* Register your solution function */
    registerTransFunction(transpose_submit, transpose_submit_desc);

    /* Register any additional transpose functions */
    //registerTransFunction(trans, trans_desc);
}

/* 
 * is_transpose - This helper function checks if B is the transpose of
 *     A. You can check the correctness of your transpose by calling
 *     it before returning from the transpose function.
 */
int is_transpose(int M, int N, int A[N][M], int B[M][N])
{
    int i, j;

    for (i = 0; i < N; i++)
    {
        for (j = 0; j < M; ++j)
        {
            if (A[i][j] != B[j][i])
            {
                return 0;
            }
        }
    }
    return 1;
}
