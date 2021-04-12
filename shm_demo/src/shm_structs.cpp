#include "shm_structs.h"
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/file.h>
#include <linux/limits.h>
#include <unistd.h>
#include "stdio.h"
int shm_quete::mem_create(const char* key, int size)
{
	int fd;
	void *addr;
	struct stat sb;
	fd = shm_open(key, O_CREAT | O_RDWR, S_IRWXU);
	if (fd == -1)
	{
		printf("mem_create create failed\n");
		return -1;
	}
	int ret = ftruncate(fd, size);
	if (ret != 0)
	{
		printf("mem_create ftruncate failed\n");
		return -1;
	}
	addr = mmap(NULL, size, PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0);
	if (addr == MAP_FAILED)
	{
		printf("mem_open mmap error\n");
		return -1;
	}
	shm_ = (char*)addr;
	start_ = (PDATA)addr;
	cursor_ = (PDATA)addr;
	end_ = (PDATA)((char*)addr + size) - 1;
    printf("cursor:%p,end:%p\n",cursor_,end_);
	close(fd);
	return 0;
}

int shm_quete::mem_open(const char * key, int size)
{
	struct stat sb;
	void* addr;
	int fd = shm_open(key, O_RDWR, S_IRWXU);
	if (fd == -1)
	{
		printf("mem_open  failed\n");
		return -1;
	}
	if (fstat(fd, &sb) == -1)
	{
		printf("mem_open fstat error\n");
		return -1;
	}

	addr = mmap(NULL, sb.st_size, PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0);
	if (addr == MAP_FAILED)
	{
		printf("mem_open mmap error\n");
		return -1;
	}
	close(fd);
	shm_ = (char*)addr;
	start_ = (PDATA)addr;
	cursor_ = (PDATA)addr;
    end_ = (PDATA)((char*)addr + size) - 1;
	return 0; 

}

void shm_quete::mem_unlink(const char*key)
{
	shm_unlink(key);
}

PDATA shm_quete::data_next()
{
	if (cursor_ != end_)
		return ++cursor_;
	else
    {
        cursor_ = start_;
        return cursor_;
    }
}
void shm_quete::reset_flag(PDATA data_ptr)
{
    data_ptr->flag = 0;
}
PDATA shm_quete::get_data()
{
	return cursor_;
}

//可写
bool shm_quete::writeable(PDATA data_ptr)
{
	return (data_ptr->flag == 0) || (data_ptr->flag ==  7);
}


//可读
bool shm_quete::readable(PDATA data_ptr)
{
    if (data_ptr->flag == 0)
    {
        return false;
    }
    else if((data_ptr->flag & index_flag_) == index_flag_)
    {
        return false;
    }
    else 
    {
        return true;
    }
}

//写完成
bool shm_quete::complete_write(PDATA data_ptr)
{
	data_ptr->flag = data_ptr->flag | 0x04;
}

//读完成
bool shm_quete::complete_read(PDATA data_ptr)
{
	//data_ptr->flag = data_ptr->flag | index_flag_;
    __sync_fetch_and_add(&(data_ptr->flag), index_flag_);
    //data_ptr->flag += index_flag_;
    printf("complete_read:data flag:%d,index_flag:%d\n",data_ptr->flag,index_flag_);
}
