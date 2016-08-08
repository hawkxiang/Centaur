/*
 * This File is part of Network Server/Producer Module.
 * NDN Producer Module.
 * build and run NDN Producer Module:
 * Initialize:
 * new Producer<Class>(Prefix,Data Name,DataInfo)
 * ----------------------------------------------
 * run Producer:
 * boost::thread(boost::bind(Producer<Class>::run,class))
 * ----------------------------------------------
 * Close:
 * finish()
 * free()
 */
#ifndef _NDN_MODULE_PRODUCER_HPP_
#define _NDN_MODULE_PRODUCER_HPP_

#include "Common.hpp"

namespace ndn {

class Producer : boost::noncopyable {
    public:
        //Producer with INFO data
        explicit Producer(const string Prefix_Name,
                            const string Data_Name,
                            const void *Data,
                            const uint32_t Size) {
            m_Type  =   INFO_DATA;
            m_Name  =   tools::createName(Prefix_Name,Data_Name);
            m_DName =   Data_Name;
            m_Fname =   tools::createInfoFileName(Data_Name,SPINFO);
            m_FD    =   create_file(Data_Name+"SP",m_Fname);
            m_Count =   0;
            //metadata to tmp file
            if (check_file(m_FD) == 1) {
                writeFileData(m_FD,0,TOCCHARP(Data),Size);
            }
        }
        //Producer with FILE data
        explicit Producer(const string Prefix_Name,
                            const string Data_Name,
                            const string Data_Path) {
            m_Type  =   FILE_DATA;
            m_Name  =   tools::createName(Prefix_Name,Data_Name);
            m_DName =   Data_Name;
            m_Fname =   Data_Path;
            m_FD    =   open_file(Data_Name+"SP",m_Fname,RDFILE);
            m_Count =   0;
        }
        explicit Producer(const string Prefix_Name,
                            const string Data_Name,
                            const key_t key,
                            const uint8_t command,
                            const string Dname){
            m_Type  =   PROXY_TEMP;
            m_Name  =   tools::createName(Prefix_Name,Data_Name);
            m_DName =   Data_Name;
            proxy_msg = (struct msg_st *)malloc(sizeof(struct msg_st));
            proxy_msg->m_type = 1;
            proxy_msg->m_command = command;
            strcpy(proxy_msg->m_name,Dname.c_str());
            msgid = msgget(key,0666| IPC_CREAT);
            if (msgid == -1){
                log_err("APP PROXY msgget() failed.");
                exit(EXIT_FAILURE);
            }
        }
    public:
        // boost::thread(boost::bind(&ndn::Producer<T1>::run,ClassName));
        void run();
        void finish();
        string get_Name();

    private:
        shared_ptr<Data>    createDataPacket(const Interest &interest);
        void                onInterest(const Interest &interest);
        void                onProxy(const Interest &interest);
        void                onRegisterFailed(const Name &prefix,const string &reason);

    private:
        string          m_Name,m_DName,m_Fname;
        int             m_Type;
        int             m_FD;
        int             m_Count;

        struct msg_st   *proxy_msg;
        int             msgid = -1;
        pthread_mutex_t proxy_lock = PTHREAD_MUTEX_INITIALIZER;
        int             running = 0;

        Face            m_Face;
        KeyChain        m_KeyChain;
};

int     add_NDN_Producer(const string prefix,
                            const string name,
                            const void* data,
                            uint32_t size);
int     add_NDN_Producer(const string prefix,
                            const string name,
                            const string path);
int     add_NDN_Producer(const string prefix,
                            const string name,
                            const key_t key,
                            const uint8_t command,
                            const string Dname);
int     del_NDN_Producer(const string prefix,const string name);

}//end namespace ndn

#endif//_NDN_MODULE_PRODUCER_HPP_
