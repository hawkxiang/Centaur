/*
 * This File is part of Network Client/Consumer Module.
 * NDN Consumer Module.
 * build and run NDN Consumer Module:
 * Initialize:
 * new Consumer<Class>(Prefix,Data Name,DataInfo)
 * ----------------------------------------------
 * Get Data:
 * opt_Retry_Data(SegPos,SegLen)
 * opt_NoRetry_Data(SegPos)
 * ----------------------------------------------
 * Close:
 * finish()
 * free()
 */
#ifndef _NDN_MODULE_CONSUMER_HPP_
#define _NDN_MODULE_CONSUMER_HPP_

#include "Common.hpp"

namespace ndn{

template<class T1>
class Consumer : boost::noncopyable {
    public:
        //Consumer with INFO data
        explicit Consumer(const string Prefix_Name,
                            const string Data_Name) {
            m_Type = INFO_DATA;
            m_Name = tools::createName(Prefix_Name,Data_Name);
            m_DName = Data_Name;
            m_Size = sizeof(T1);
            m_Seg = 1;
            m_Fname = tools::createInfoFileName(Data_Name,CCINFO);
            m_FD = create_file(Data_Name+"CC",m_Fname);
            m_interestLifetime = time::milliseconds(5000);
        }
        //Consumer with FILE data
        explicit Consumer(const string Prefix_Name,
                            const string Data_Name,
                            const string Data_Path,
                            const uint32_t Seg_Num) {
            m_Type = FILE_DATA;
            m_Name = tools::createName(Prefix_Name,Data_Name);
            m_DName = Data_Name;
            m_Size = sizeof(T1);
            m_Seg = Seg_Num;
            m_Fname = Data_Path;
            m_FD = create_file(m_DName+"CC",m_Fname);
            m_interestLifetime = time::milliseconds(5000);
        }
        explicit Consumer(const string Prefix_Name,
                            const string Data_Name,
                            const uint32_t Seg_Num){
            m_Type = FILE_DATA;
            m_Name = tools::createName(Prefix_Name,Data_Name);
            m_DName = Data_Name;
            m_Size = sizeof(T1);
            m_Seg = Seg_Num;
            m_FD = -1;
        }

    public:
        void    setLifetime_mseconds(uint32_t Ti);
        size_t  opt_Retry_Data(uint32_t Seg_Pos,uint32_t Seg_Len);
        size_t  opt_Noretry_Data(uint32_t Seg_Pos = 0);
        void    finish();
        int     getData(T1 *Data);

    private:
        void        SchedulePacket(uint32_t Seg_Begin,uint32_t Seg_End);
        Interest    createInterestPacket(uint32_t seg_num);
        void        onData_Next(Data &data,uint32_t Seg_End);
        void        onData(Data &data);
        void        onTimeOut_Retry(const Interest &interest,uint32_t Seg_End);
        void        onTimeOut_Noretry(const Interest &interest);

    private:
        string              m_Name,m_Fname,m_DName;
        uint32_t            m_Size,m_Seg;
        int                 m_Type;
        int                 m_FD;
        pthread_mutex_t     m_Lock = PTHREAD_MUTEX_INITIALIZER;

        time::milliseconds  m_interestLifetime;
        Face                m_Face;

        int                 Consumer_Flag = 0;
        uint32_t            CountSize;
};

}//end namespace ndn

#define _NDN_MODULE_CONSUMER_CPP_
#include "Consumer.cpp"

#endif//_NETWORK_CC_MODULE_CONSUMER_HPP_
