#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <pthread.h>

int points_in_circle = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

double random_double(struct drand48_data *buffer) {
	double result;
	drand48_r(buffer, &result);
	return result;
}

void* generate_points(void *arg) {
	int points = *(int*) arg;
	struct drand48_data buffer;
	srand48_r(time(NULL) ^ pthread_self(), &buffer); // Unique seed for each thread
	int count = 0;
	for (int i = 0; i < points; ++i) {
		double x = random_double(&buffer) * 2.0 - 1.0;
		double y = random_double(&buffer) * 2.0 - 1.0;
		if (sqrt(x * x + y * y) <= 1.0) {
			++count;
		}
	}
	pthread_mutex_lock(&mutex);
	points_in_circle += count;
	pthread_mutex_unlock(&mutex);
	return NULL;
}

int main(int argc, char *argv[]) {
	if (argc != 3) {
		fprintf(stderr,
				"Usage: %s <integer value for NUMBER_OF_DARTS> <integer value for NUMBER_OF_THREADS>\n",
				argv[0]);
		return 1;
	}

	int num_of_Points = atoi(argv[1]);
	int num_of_Threads = atoi(argv[2]);

	if (num_of_Points < 5000000) {
		fprintf(stderr, "The number of darts must be >= 5000000\n");
		return 1;
	}

	if (num_of_Threads < 2) {
		fprintf(stderr, "The number of threads must be >= 2\n");
		return 1;
	}

	srand((unsigned int) time(NULL));

	pthread_t threads[num_of_Threads];
	int points_per_thread = num_of_Points / num_of_Threads;

	for (int i = 0; i < num_of_Threads; ++i) {
		pthread_create(&threads[i], NULL, generate_points, &points_per_thread);
	}

	for (int i = 0; i < num_of_Threads; ++i) {
		pthread_join(threads[i], NULL);
	}

	double pi = 4.0 * points_in_circle / num_of_Points;
	printf("Pi = %lf\n", pi);

	pthread_mutex_destroy(&mutex);
	return 0;
}
