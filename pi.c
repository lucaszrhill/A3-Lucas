#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <pthread.h>

const int num_of_Points = 50000000;
const int num_of_Threads = 4; // Set a fixed number of threads
int points_in_circle = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

double random_double() {
	return rand() / (double) (RAND_MAX + 1);
}

void* generate_points(void *arg) {
	int points = *(int*) arg;
	int count = 0;
	for (int i = 0; i < points; ++i) {
		double x = random_double() * 2.0 - 1.0;
		double y = random_double() * 2.0 - 1.0;
		if (sqrt(x * x + y * y) <= 1.0) {
			++count;
		}
	}
	pthread_mutex_lock(&mutex);
	points_in_circle += count;
	pthread_mutex_unlock(&mutex);
	return NULL;
}

int main() {
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
	printf("Estimated value of Pi: %lf\n", pi);

	pthread_mutex_destroy(&mutex);
	return 0;
}
