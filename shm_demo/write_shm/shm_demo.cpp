#include "pthread.h"
#include "memory"
#include "shm_structs.h"
#include "stdio.h"
#include "nmmintrin.h"
#include "string.h"
#include "stdlib.h"
#include <linux/types.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
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
	for (vector<int>::const_iterator it = vec.begin();it != vec.end();it++)
	{
		ss << (*it)<< "," << "\n";
		outfile.write(ss.str().c_str(), ss.str().length());
	}
	
	outfile.close();
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

	shm_quete *shm = new shm_quete(0x01);
	int size_data = nsize * sizeof(DATA);
	cout << size_data << endl;
	shm->mem_create(SHM_KEY, size_data);
	PDATA data = shm->get_data();
    int sum_n = 0;
	std::vector<int> v_result;
	__u64 begin = rdtsc();
	for (int i = 0; i < loop_count; i++)
	{
		__u64 begin_w = rdtsc();
        while (shm->writeable(data) == false);
        shm->reset_flag(data);
		data->value = i;
		shm->complete_write(data);
		data = shm->data_next();
        sum_n += i;
		__u64 end_w = rdtsc();
		v_result.push_back(end_w - begin_w);
	}
	__u64 end = rdtsc();
	cout << "shm test finished." <<  "consume:" << end - begin << " CPU cycles." << endl;
	vecwrite2file(v_result);
    shm->mem_unlink(SHM_KEY);
    cout << "complete,sum:" << sum_n << endl;
}
