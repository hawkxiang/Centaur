/*
 * This File is part of Netowrk Server/Producer Module.
 * IP Server Module.
 * build and run IP Server Module(TCP mode):
 * Initialize:
 * Step 1: init_server_pool(IP port)
 * Step 2: run_TCP_Server(backlog)
 * ------------------------------------------
 * Step add Server: add_TCP_Server(Data name,DataInfo)
 * Step del Server: del_TCP_Server(Data name)
 * ------------------------------------------
 * Close
 * Step 1: close_TCP_Server()
 */

#ifndef _IP_MODULE_SERVER_HPP_
#define _IP_MODULE_SERVER_HPP_

#include "Common.hpp"

namespace ip {

class Server : boost::noncopyable {
    public:
        //Server with INFO data
        explicit Server(const string Data_Name,
                        const void* Data,
                        const uint32_t Size) {
            m_Type = INFO_DATA;
            m_Dname = Data_Name;
            m_NameLen = m_Dname.size();
            m_Fname = tools::createInfoFileName(Data_Name,SPINFO);
            m_Times=0;
            m_FD = create_file(Data_Name+"@SP",m_Fname);
            //metadata to tmp file
            if (check_file(m_FD) == 1) {
                writeFileData(m_FD,0,TOCCHARP(Data),Size);
            }
        }
        //Server with FILE data
        explicit Server(const string Data_Name,
                        const string Data_Path) {
            m_Type = FILE_DATA;
            m_Dname = Data_Name;
            m_NameLen = m_Dname.size();
            m_Fname = Data_Path;
            m_Times=0;
            m_FD = open_file(Data_Name+"@SP",m_Fname,RDFILE);
        }
    public:
        string          get_Dname();
        int             get_FD();
        void            setmTimes(uint32_t mTimes);
        uint32_t        getmTimes();
        void            finish();

    private:
        string      m_Dname,m_Fname;
        uint32_t    m_NameLen;
        uint32_t    m_Times;
        int         m_Type;
        int         m_FD;
};

void        init_Server_pool(int Port);
int         run_TCP_Server(int backlog);
void        close_TCP_Server();
int         add_TCP_Server(const string name,const void* Data,uint32_t Size);
int         add_TCP_Server(const string name,const string Path);
int         add_TCP_Server(const string name,const string Path,const uint32_t mTimes);
int         del_TCP_Server(const string name);

}//end namespace ip

#endif//_IP_MODULE_SERVER_HPP_
