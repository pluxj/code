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
		"rdtscp":"=a"(lo), "=d"(hi)
               :
               : "%rcx"
	);
	return (__u64)hi << 32 | lo;
}

void vecwrite2file(const vector<uint64_t>& vec)
{
	ofstream outfile;
	char file[64] = { 0 };
	snprintf(file, 64, "%d.csv", getpid());
	outfile.open(file, std::fstream::app | std::fstream::out);
	if (!outfile)
	{
		exit(1);
	}
    cout << "vec result:" << vec.size() << endl;
	for (vector<uint64_t>::const_iterator it = vec.begin(); it != vec.end(); it++)
	{
	    stringstream ss;
		ss << (*it) << "," << "\n";
		outfile.write(ss.str().c_str(), ss.str().length());
        ss.clear();
	}

	outfile.close();
}

void consume_mem(int index_flag, int size, int loop_count)
{
	shm_quete *shm = new shm_quete(index_flag);
	cout << "index flag:" << index_flag << endl;
	cout << "consume pid:" << getpid() << endl;
    cout << "loop_count: " << loop_count << endl;
	int nsize = size * sizeof(DATA);
	shm->mem_open(SHM_KEY, nsize);
	int sum_n = 0;
	int i = 0;
	PDATA data = shm->get_data();
	std::vector<uint64_t> v_result;
    v_result.resize(loop_count);
	__u64 begin = rdtsc();
	for (int i = 0; i < loop_count; i++)
	{
		while (shm->readable(data) == false);
		sum_n += data->value;
		shm->complete_read(data);
		uint64_t timestamp = rdtsc();
		data = shm->data_next();
		
		v_result[i] = timestamp;

	}
	__u64 end = rdtsc();
	cout << "consume shm test finished." << "consume:" << end - begin << " CPU cycles." << endl;
	cout << "sum_n:" << sum_n << endl;
    cout << "pid:"<<getpid() << " ,result size:" << v_result.size() << endl;
	vecwrite2file(v_result);
	exit(0);
}

void produce(int index_flag, int size, int loop_count)
{
	shm_quete *shm = new shm_quete(0x01);
	int size_data = size * sizeof(DATA);
	cout << size_data << endl;
	shm->mem_create(SHM_KEY, size_data);
	PDATA data = shm->get_data();
	int sum_n = 0;
	std::vector<uint64_t> v_result;
    v_result.resize(loop_count);
	__u64 begin = rdtsc();
	cout << "produce pid:" << getpid() << endl;
    cout << "loop_count: " << loop_count << endl;
	for (int i = 0; i < loop_count; i++)
	{
		while (shm->writeable(data) == false);
		shm->reset_flag(data);
		data->value = i;
		shm->complete_write(data);
		uint64_t timestamp = rdtsc();
		data = shm->data_next();
		sum_n += i;
		v_result[i] = timestamp;
	}
	__u64 end = rdtsc();
	cout << "produce shm test finished." << "consume:" << end - begin << " CPU cycles." << endl;
    cout << "pid: "<<getpid()<<" ,result size:" << v_result.size() << endl;
	vecwrite2file(v_result);
//	shm->mem_unlink(SHM_KEY);
	cout << "complete,sum:" << sum_n << endl;
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

	for (int i = 0; i < 3; i++)
	{
		int pid = fork();
		switch (pid)
		{
		case -1:
			exit(-1);
		case 0:
			if (i == 2)
			{
				produce(i, nsize, loop_count);
				break;
			}
			consume_mem(i + 1, nsize, loop_count);
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
