#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define CMDLINE_ERROR 1
#define PTH_CRT_ERROR 2
#define BARR_ERROR 3
#define THR_JOIN_ERROR 3

int TIME = 0;

int cmdline_error();
int pthread_create_error(int i);
int pthread_barrier_error();
void* task(void* arg);
int test(void* arg);
void check_threads_results(int *results, int *success_threads, int* time_of_success, int size, int* num_of_failed);
int thread_join_error(int j);

typedef struct
{
	int *arr;
	int num;
} info;

int main(int argc, char* argv[])
{
	if (argc != 2)
		return cmdline_error();
	int num_threads;
	num_threads = atoi(argv[1]);
	if (num_threads == 0)
		return cmdline_error();
	
	srand(time(NULL));
	printf("Dispatcher started...\n");
	sleep(1);
	pthread_t *threads;
	threads = malloc(num_threads * sizeof(pthread_t));
	
	int i = 0;
	int j = 0;
	int try = 0;
	int *success_threads;
	success_threads = malloc(num_threads * sizeof(int));	
	int *time_of_success;
	time_of_success = malloc(num_threads * sizeof(int));
	for (i = 0; i < num_threads; ++i)
	{
		success_threads[i] = 0;
		time_of_success[i] = 0;
	}
	int *results;
	results = malloc(num_threads * sizeof(int));
	info *data;
	data = malloc(num_threads * sizeof(info));
	for (i = 0; i < num_threads; ++i)
	{
		data[i].arr = results;
		data[i].num = i;
	}	
	int num_of_failed = num_threads;

	while (num_of_failed > 0)
	{
		TIME++;
		for (i = 0; i < num_threads; ++i)
			if (success_threads[i] == 0)
			{
				try = pthread_create(&threads[i], NULL, task, &data[i]);
				if (try == 0)
					{
						printf("Thread#%d started...\n", i);
						sleep(1);
					}
				else
					return pthread_create_error(i);
			}

		check_threads_results(results, success_threads, time_of_success, num_threads, &num_of_failed);
		for (j = 0; j < num_threads; ++j)
		{
			if ((success_threads[j] == 1) && (time_of_success[j] == TIME))
			{
				try = pthread_join(threads[j], NULL);
				if (try == 0) printf("Thread#%d finished\n", j);
					else return thread_join_error(j);
			}
		}
	}
	
	printf("Your random even numbers:\n");
	for (j = 0; j < num_threads; ++j)
			printf("%d\n", results[j]);

	free(threads);
	free(results);
	free(time_of_success);
	free(success_threads);
	free(data);

	printf("Dispatcher finished\n");
	sleep(1);

	return 0;
}

int cmdline_error()
{
	printf("Command line error");
	return CMDLINE_ERROR;
}

int pthread_create_error(int i)
{
	printf("Pthread_create %d error\n", i);
	return PTH_CRT_ERROR;
}

int pthread_barrier_error()
{
	printf("Pthread_barrier error\n");
	return BARR_ERROR;
}

int thread_join_error(int j)
{
	printf("Thread#%d join error\n", j);
	return THR_JOIN_ERROR;
}

void* task(void* arg)
{
	info *data = (info*) arg;
	(data->arr)[data->num] = rand();
}

int test(void* arg)
{
	int *x = (int*) arg;
	if ((*x % 2) == 0) return 1;
		else return 0;
}

void check_threads_results(int* results, int *success_threads, int* time_of_success, int size, int *num_of_failed)
{
	int i = 0;
	int count = 0;
	for (i = 0; i < size; ++i)
		{
			if (success_threads[i] == 0)
			{
				if (test(&results[i])==1)
				{
					success_threads[i] = 1;
					time_of_success[i] = TIME;
					*num_of_failed -= 1;
					printf("Thread#%d scored a success\n", i);
					sleep(1);
				}
				else
				{
					printf("Thread#%d failed\n", i);
					sleep(1);
				}
			}
		}
}

