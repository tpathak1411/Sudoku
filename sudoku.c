#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include <string.h>

int no_of_threads = 0;
# define MAX_THREADS 500

// To store parameters for solveSudoku
struct fun_params {
    int grid[36][36];
    int row, col, N;
    int* b;
};
    
// To print sudoku
void print_grid(int grid[36][36], int n)
{
    for (int i = 0; i < n; i++)	{
        for (int j = 0; j < n; j++)
            printf("%d\t", grid[i][j]);
        printf("\n");
    }
}

int isSafe(int grid[36][36], int row, int col, int num, int n) {

	// Check for same num in the same column
	for (int x = 0; x <= n-1; x++)
		if (grid[x][col] == num)
			return 0;
    
    // Check for same num in the same row
	for (int x = 0; x <= n-1; x++)
		if (grid[row][x] == num)
			return 0;

    // Check for same num in smaller square
    int x = sqrt(n);
	int startRow = row - row%x;
    int startCol = col - col%x;

	for (int i = 0; i < x; i++)
		for (int j = 0; j < x; j++)
			if (grid[+startRow][j+startCol] == num)
				return 0;

	return 1;
}

// multi-threaded sudoku solver
void* solveSudoku(void* args)
{
    struct fun_params* params = (struct fun_params*) args;
    
    if (params->row >= params->N-1 && params->col >= params->N) {
        *(params->b) = 1;
        print_grid(params->grid, params->N);
        return NULL;
    }
    
    if (params->col >= params->N) {
        params->row++;
        params->col = 0;
    }
    
    if (params->grid[params->row][params->col] > 0) {
        params->col++;
        solveSudoku(args);
    }
    
    else {
        if(no_of_threads < MAX_THREADS) {
            // use thread
            pthread_t threads[36];
            struct fun_params next_param[params->N];

            int n = 0;
            for (int num = 1; num <= params->N; num++) {

                if (isSafe(params->grid, params->row, params->col, num, params->N)) {
                    
                    params->grid[params->row][params->col] = num;
                    
                    if(!*(params->b)) {
                        memcpy(next_param[n].grid, params->grid, 36*36*sizeof(int)); 
                        next_param[n].row=params->row; 
                        next_param[n].col=params->col+1; 
                        next_param[n].N=params->N; 
                        next_param[n].b=params->b;
                        
                        pthread_create(&threads[n], NULL, solveSudoku, (void*)&next_param[n]);
                        n++;
                        no_of_threads++;
                    }
                }
            }
            for(int i=0; i<n; i++) {
                pthread_join(threads[i], NULL);
            }

            no_of_threads -= n;
        }
        else {
            // recursive call
            for (int num = 1; num <= params->N; num++) {

                if (isSafe(params->grid, params->row, params->col, num, params->N)) {
                    
                    params->grid[params->row][params->col] = num;
                    
                    if(!*(params->b)) {
                        struct fun_params fun_param;
                        memcpy(fun_param.grid, params->grid, 36*36*sizeof(int)); 
                        fun_param.row=params->row; 
                        fun_param.col=params->col+1; 
                        fun_param.N=params->N; 
                        fun_param.b=params->b;

                        solveSudoku(&fun_param);
                        params->grid[params->row][params->col] = 0;
                    }
                }
            }
        }
    }
    
    return NULL;
}

// read file
void read_grid_from_file(int size, char *ip_file, int grid[36][36]) {
	FILE *fp;
	int i, j;
	fp = fopen(ip_file, "r");
	for (int i = 0; i < size; i++) 
        for (int j = 0; j < size; j++) {
            fscanf(fp, "%d", &grid[i][j]);
    }
} 
    
int main(int argc, char *argv[])
{
    if (argc != 3) {
		printf("Usage: ./sudoku.out grid_size inputfile");
		exit(-1);
	}

    int size = atoi(argv[1]);
    FILE *fp = fopen(argv[2], "r");
    
    int grid[36][36];

    read_grid_from_file(size, argv[2], grid);

    int b = 0;
    struct fun_params params;
    memcpy(params.grid, grid, 36*36*sizeof(int));
    params.row = 0; 
    params.col = 0; 
    params.N = size; 
    params.b = &b;

    pthread_t thread;
    pthread_create(&thread, NULL, solveSudoku, &params);
    pthread_join(thread, NULL);
    
    return 0;
}