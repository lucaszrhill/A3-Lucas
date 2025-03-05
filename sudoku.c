#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>

#define SIZE 9

int grid[SIZE][SIZE];
bool row_valid = true;
bool col_valid = true;
bool subgrid_valid[SIZE] = { true, true, true, true, true, true, true, true,
		true };

void* check_rows(void *arg) {

	for (int i = 0; i < SIZE; i++) {

		int row[SIZE + 1] = { 0 };

		for (int j = 0; j < SIZE; j++) {

			int num = grid[i][j];
			if (row[num]) {
				row_valid = false;
				return NULL;
			}

			row[num] = 1;
		}
	}
	return NULL;
}

void* check_columns(void *arg) {

	for (int i = 0; i < SIZE; i++) {

		int col[SIZE + 1] = { 0 };

		for (int j = 0; j < SIZE; j++) {

			int num = grid[j][i];
			if (col[num]) {
				col_valid = false;
				return NULL;
			}

			col[num] = 1;
		}
	}
	return NULL;
}

void* check_subgrid(void *arg) {

	int index = *(int*) arg;
	int start_row = (index / 3) * 3;
	int start_col = (index % 3) * 3;
	int subgrid[SIZE + 1] = { 0 };

	for (int i = 0; i < 3; i++) {

		for (int j = 0; j < 3; j++) {

			int num = grid[start_row + i][start_col + j];

			if (subgrid[num]) {
				subgrid_valid[index] = false;
				return NULL;
			}

			subgrid[num] = 1;
		}
	}
	return NULL;
}

int main(int argc, char *argv[]) {

	if (argc != 2) {
		fprintf(stderr, "Usage: %s <input_file>\n", argv[0]);
		return EXIT_FAILURE;
	}

	FILE *file = fopen(argv[1], "r");

	if (file == NULL) {
		perror("Error opening file");
		return EXIT_FAILURE;
	}

	for (int i = 0; i < SIZE; i++) {

		for (int j = 0; j < SIZE; j++) {

			if (fscanf(file, "%d", &grid[i][j]) != 1) {
				perror("Error reading file");
				fclose(file);
				return EXIT_FAILURE;
			}
		}
	}

	fclose(file);

	printf("Sudoku Puzzle Solution is:\n");

	for (int i = 0; i < SIZE; i++) {
		for (int j = 0; j < SIZE; j++) {
			printf("%d ", grid[i][j]);
		}
		printf("\n");
	}

	pthread_t threads[SIZE + 2];
	int indices[SIZE];

	for (int i = 0; i < SIZE; i++) {

		indices[i] = i;
		pthread_create(&threads[i], NULL, check_subgrid, &indices[i]);
	}

	pthread_create(&threads[SIZE], NULL, check_rows, NULL);
	pthread_create(&threads[SIZE + 1], NULL, check_columns, NULL);

	for (int i = 0; i < SIZE + 2; i++) {

		pthread_join(threads[i], NULL);
	}

	bool valid = row_valid && col_valid;

	for (int i = 0; i < SIZE; i++) {
		valid &= subgrid_valid[i];
	}

	if (valid) {
		printf("Sudoku puzzle is valid\n");
	} else {
		printf("Sudoku puzzle is invalid\n");
	}

	return EXIT_SUCCESS;
}
