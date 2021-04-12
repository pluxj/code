#include "pthread.h"
#include "memory"
#include "shm_structs.h"
#include "sys/types.h"
#include <unistd.h>
#include <iostream>
#include "stdio.h"
#include <sys/wait.h>
using namespace std;

void consume_mem(int index_flag)
{
	shm_quete *shm = new shm_quete(index_flag);
	int nsize = 4096 * sizeof(DATA);
	shm->mem_open(SHM_KEY, nsize);
	int sum_n = 0;
	int i = 0;
	PDATA data = shm->get_data();
	for (int i = 0; i < 100; i++)
	{
		(data->value)++;
	}
	exit(0);
}

int main(int argc, char* argv)
{
	int index_flag = 0x01;
	for (int i = 0; i < 2; i++)
	{
		int pid = fork();
		switch (pid)
		{
		case -1:
			exit(-1);
		case 0:
			consume_mem(i + 1);
		default:
			break;
		}
	}
	int numDead = 0;
	int child_pid = 0;
	for (;;)
	{
		child_pid = wait(NULL);
		if (child_pid == -1)
		{
			exit(-1);
		}
		numDead++;
		printf("%d exit,numDead:%d\n", child_pid, numDead);
	}

}