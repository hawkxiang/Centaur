#ifndef _STORAGE_MODULE_HPP_
#define _STORAGE_MODULE_HPP_

#include "../Common.hpp"
#include "../tools/Common.hpp"

int         init_Storage_Module();
int         open_file(const string name,const string path,int oflag);
int         create_file(const string name,const string path);
int         close_file(const string name);
int         close_file(int fd);
int         delete_file(int fd);
int         check_file(int fd);

uint32_t    readFileData(int fd,uint32_t Pos,uint8_t *Data,uint32_t Len);
uint32_t    writeFileData(int fd,uint32_t Pos,const uint8_t *Data,uint32_t Len);

struct Storage_FD {
    int                 fd_count;
    pthread_mutex_t     fd_lock;
};

#endif//_STORAGE_MODULE_HPP_
