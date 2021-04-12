#include "pthread.h"
#include "memory"
#include "shm_structs.h"
#include "sys/types.h"
#include <unistd.h>
#include <linux/types.h>
#include <iostream>
#include "stdio.h"
#include "stdlib.h"
#include <sys/wait.h>
#include <fstream>
#include <vector>
#include <sstream>
using namespace std;

uint64_t rdtsc()
{
	__u32 lo, hi;

	__asm__ __volatile__
	(
		"rdtsc":"=a"(lo), "=d"(hi)
	);
	return (__u64)hi << 32 | lo;
}

void vecwrite2file(const vector<int>& vec)
{
	ofstream outfile;
	char file[64] = { 0 };
	snprintf(file, 64, "%d.csv", getpid());
	outfile.open(file, std::fstream::app | std::fstream::out);
	if (!outfile)
	{
		exit(1);
	}
	stringstream ss;
	for (vector<int>::const_iterator it = vec.begin(); it != vec.end(); it++)
	{
		ss << (*it) << "," << "\n";
		outfile.write(ss.str().c_str(), ss.str().length());
	}

	outfile.close();
}

void consume_mem(int index_flag,int size,int loop_count)
{
	shm_quete *shm = new shm_quete(index_flag);
    cout << "index flag:" << index_flag << endl;
	int nsize = size * sizeof(DATA);
	shm->mem_open(SHM_KEY, nsize);
	int sum_n = 0;
	int i = 0;
	PDATA data = shm->get_data();
	std::vector<int> v_result;
	__u64 begin = rdtsc();
	for (int i = 0; i < loop_count; i++)
	{
		__u64 begin_w = rdtsc();
		while (shm->readable(data) == false);
		sum_n += data->value;
		shm->complete_read(data);
		data = shm->data_next();
		__u64 end_w = rdtsc();
		v_result.push_back(end_w - begin_w);

	}
	__u64 end = rdtsc();
	cout << "shm test finished." << "consume:" << end - begin << " CPU cycles." << endl;
    cout << "sum_n:" << sum_n << endl;
	vecwrite2file(v_result);
	exit(0);
}
int main(int argc, char* argv[])
{
	int nsize = 2;
	int loop_count = 100;
	if (argc > 1)
	{
		nsize = atoi(argv[1]);
		cout << "nsize:" << nsize << endl;
	}
	if (argc > 2)
	{
		loop_count = atoi(argv[2]);
		cout << "nsize:" << nsize << endl;
	}

	for (int i = 0; i < 2; i++)
	{
		int pid = fork();
		switch (pid)
		{
		case -1:
			exit(-1);
		case 0:
			consume_mem(i+1,nsize,loop_count);
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
