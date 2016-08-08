/*
 * This File is part of Network Client/Consumer Module.
 * IP Client Module.
 * build and run IP Client Module(TCP mode):
 * Initialize:
 * new ip::Client<Class>(Server address,Data name)
 * -----------------------------------------------
 * Get Data:
 * run_TCP()
 * opt_TCP(SegPos,SegLen)
 * getData() for Info Data
 * -----------------------------------------------
 * Close
 * finish_TCP()
 * free()
 */

#ifndef _IP_MODULE_CLIENT_HPP_
#define _IP_MODULE_CLIENT_HPP_

#include "Common.hpp"

namespace ip {

static inline int connect_retry(int domain,int type,int protocol,
                            const struct sockaddr *addr,socklen_t alen);

template<class T1>
class Client {
    public:
        //Client with INFO data
        explicit Client(const IP_v4_Address Serv_Addr,
                        const string Data_Name) {
            m_Type = INFO_DATA;
            m_DName = Data_Name;
            m_NameLen = m_DName.size();
            init_serv_addr(Serv_Addr);
            m_Size = sizeof(T1);
            m_Seg = 1;
            m_Fname = tools::createInfoFileName(Data_Name,CCINFO);
            m_FD = create_file(Data_Name+"CC",m_Fname);
        }
        //Client with FILE data
        explicit Client(const IP_v4_Address Serv_Addr,
                        const string Data_Name,
                        const string Data_Path,
                        const uint32_t Seg_Num) {
            m_Type = FILE_DATA;
            m_DName = Data_Name,
            m_NameLen = m_DName.size();
            init_serv_addr(Serv_Addr);
            m_Size = sizeof(T1);
            m_Seg = Seg_Num;
            m_Fname = Data_Path;
            m_FD = create_file(Data_Name+"CC",m_Fname);
        }

    public:
        int         run_TCP();
        uint32_t    opt_TCP(uint32_t Seg_Pos,uint32_t Seg_Len);
        void        finish_TCP();
        uint32_t    getData(T1 *Data);

    private:
        void        init_serv_addr(const IP_v4_Address Serv_Addr);
        uint32_t    receiveData(int fd,uint32_t Pos);
        void        closeReq();

    private:
        string              m_Fname,m_DName;
        uint32_t            m_NameLen,m_Size,m_Seg;
        int                 m_Type;
        int                 m_FD;

        struct sockaddr_in  m_serv_adr;
        int                 m_TCP_sock;

        int                 TCP_Count = 0;
        int                 Client_Flag = 0;
        pthread_mutex_t     TCP_Lock = PTHREAD_MUTEX_INITIALIZER;
};

}//end namespace ip

#define _IP_MODULE_CLIENT_CPP_
#include "Client.cpp"

#endif//_IP_MODULE_CLIENT_HPP_
