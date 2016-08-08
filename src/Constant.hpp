#ifndef _CONSTANT_HPP_
#define _CONSTANT_HPP_

#ifdef __APPLE__
    #define __SYSTEM__ 2
#elif __Linux
    #define __SYSTEM__ 1
#elif __unix
    #define __SYSTEM__ 1
#elif __posix
    #define __SYSTEM__ 1
#else
    #define __SYSTEM__ 0
#endif

#define DATAFILEDIR     "/tmp/TestDataDIR"
#define INFOFILEDIR     "/tmp/TestInfoDIR"
#define CCINFO          ".CCINFO"
#define SPINFO          ".SPINFO"

#define RWFILE      O_RDWR
#define RDFILE      O_RDONLY

#define FILE_MODE   (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)
#define DIR_MODE    (FILE_MODE | S_IXUSR | S_IXGRP | S_IXOTH)

#define TOCHARP(x) reinterpret_cast<uint8_t*>(x)
#define TOCCHARP(x) reinterpret_cast<const uint8_t*>(x)

using std::string;

const int __DEFAULT_PORT__ = 6699;

const int INFO_DATA  = 0;
const int FILE_DATA  = 1;
const int PROXY_TEMP = 2;

struct IP_v4_Address
{
    char Address[20];
    int Port = __DEFAULT_PORT__;
};

/*
 * Sturct Data.
 * DATA_XK for size.
 * Data_XK for structure.
 * 1KB = 1024.
 * 2KB = 2048.
 * 4KB = 4096.
 * 8KB = 8192.
 */
const int   DATA_1K = 1024;
const int   DATA_2K = 2048;
const int   DATA_4K = 4096;
const int   DATA_8K = 8192;
struct Data_1K {
    char    m_Data[DATA_1K];
};
struct Data_2K {
    char    m_Data[DATA_2K];
};
struct Data_4K {
    char    m_Data[DATA_4K];
};
struct Data_8K {
    char    m_Data[DATA_8K];
};

struct File_Data {
    uint32_t    Size;
    uint32_t    Seg;
};

#define NAMELEN 256
#define PATHLEN 512

struct msg_st{
    long        m_type;
    uint8_t     m_command;
    char        m_name[NAMELEN];
    char        m_path[PATHLEN];
    uint32_t    m_args;
};

#define MSGSIZE (sizeof(msg_st) - sizeof(long))

#endif//_CONSTANT_HPP_
