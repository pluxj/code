#pragma once
#ifndef _SHM_STRUCT_
#define _SHM_STRUCT_

typedef struct Data
{
	int value;
    volatile int flag;
}DATA,*PDATA;

typedef struct ShareValue
{
	int flag;
	DATA value;
}SAHREVALE,*PSHAREVALUE;

typedef struct head_t
{
	char* data_start;
	char* data_end;
};

#define SHM_KEY "shm_key"

class shm_quete
{
public:
	shm_quete(int index_flag) :index_flag_(index_flag)
	{

	}
	int mem_create(const char* key, int size);

	int mem_open(const char * key, int size);

	void mem_unlink(const char*key);

    void reset_flag(PDATA data_ptr);

	PDATA data_next();
	PDATA get_data();
	//可写
	bool writeable(PDATA data_ptr);
	//可读
	bool readable(PDATA data_ptr);

	//写完成
	bool complete_write(PDATA data_ptr);

	//读完成
	bool complete_read(PDATA data_ptr);
private:
	char *shm_;
	PDATA start_;
	PDATA end_;
	PDATA cursor_;
	int index_flag_;
};
#endif

