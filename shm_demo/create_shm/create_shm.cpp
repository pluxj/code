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


int main(int argc, char* argv[])
{

	int nsize = 2;
	int loop_count = 100;
	if (argc > 1)
	{
		nsize = atoi(argv[1]);
		cout << "nsize:" << nsize << endl;
	}

	shm_quete *shm = new shm_quete(0x01);
	int size_data = nsize * sizeof(DATA);
	cout << size_data << endl;
	shm->mem_create(SHM_KEY, size_data);
	return 0;
}