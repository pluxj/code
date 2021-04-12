#include "pthread.h"
#include "memory"
#include "shm_structs.h"
#include "sys/types.h"
#include <unistd.h>
#include <iostream>
#include "stdio.h"
#include <sys/wait.h>
using namespace std;

int main(int argc, char* argv)
{
	shm_quete *shm = new shm_quete(0);
	int nsize = 2 * sizeof(DATA);
	shm->mem_open(SHM_KEY, nsize);
	PDATA data = shm->get_data();
	for (int i = 0; i < 2; i++)
	{
		cout << "value:" << data->value << ",flag:" << data->flag << endl;
		data++;
	}

}
