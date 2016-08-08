/*
 * Complete NDN Consumer Module.
 */
#ifndef _NDN_MODULE_CONSUMER_CPP_
#include "Consumer.hpp"

#else

namespace ndn {

/*
 * set interest packet lifetime
 */
template<class T1> void
Consumer<T1>::setLifetime_mseconds(uint32_t Ti) {
    this->m_interestLifetime = time::milliseconds(Ti);
}

/*
 * get Data with retry data
 */
template<class T1> size_t
Consumer<T1>::opt_Retry_Data(uint32_t Seg_Pos,uint32_t Seg_Len) {
    uint32_t    Begin,End;
    size_t      Ret;
    pthread_mutex_lock(&m_Lock);
    CountSize = 0;
    Begin = Seg_Pos;
    End = Seg_Pos + Seg_Len;
    if (End > m_Seg) {
        End = m_Seg;
    }
    SchedulePacket(Begin,End);
    m_Face.processEvents();
    Ret = CountSize;
    pthread_mutex_unlock(&m_Lock);
    return Ret;
}

/*
 * get Data without rerty data
 */
template<class T1> size_t
Consumer<T1>::opt_Noretry_Data(uint32_t Seg_Pos) {
    size_t  Ret;
    pthread_mutex_lock(&m_Lock);
    CountSize = 0;
    m_Face.expressInterest(createInterestPacket(Seg_Pos),
                            bind(&Consumer::onData,this,_2),
                            bind(&Consumer::onTimeOut_Noretry,this,_1));
    m_Face.processEvents(this->m_interestLifetime);
    Ret = CountSize;
    pthread_mutex_unlock(&m_Lock);
    return Ret;
}

/*
 * finish NDN serve and close REQ
 * close file and delete info file
 */
template<class T1> void
Consumer<T1>::finish() {
    m_Face.shutdown();
    Consumer_Flag = -1;
    if (m_FD == -1)
        return;
    if (m_Type == INFO_DATA) {
        delete_file(m_FD);
    } else {
        close_file(m_FD);
    }
}

template<class T1> int
Consumer<T1>::getData(T1 *Data) {
    if (Consumer_Flag == -1) {
        log_err("Consumer() can't get Data\n");
        return (-1);
    }
    if (m_Type == FILE_DATA) {
        log_err("Consumer() it's file Data\n");
        return (-1);
    }
    return ((readFileData(m_FD,0,TOCHARP(Data),m_Size)));
}

/*
 * get data packet schedule
 */
template<class T1> void
Consumer<T1>::SchedulePacket(uint32_t Seg_num,uint32_t Seg_End) {
    if (Seg_num == Seg_End)
        return;
    m_Face.expressInterest(createInterestPacket(Seg_num),
                            bind(&Consumer::onData_Next,this,_2,Seg_End),
                            bind(&Consumer::onTimeOut_Retry,this,_1,Seg_End));
}

/*
 * create intereset packet
 */
template<class T1> Interest
Consumer<T1>::createInterestPacket(uint32_t seg_num) {
    shared_ptr<Name> interestName = make_shared<Name>(m_Name);
    shared_ptr<Interest> interestPacket = make_shared<Interest>();
    interestName->appendNumber(m_Size);
    interestName->appendSegment(seg_num);
    interestPacket->setName(*interestName);
    interestPacket->setInterestLifetime(m_interestLifetime);
    if (m_Type == INFO_DATA){
        interestPacket->setMustBeFresh(true);
    }
    return *interestPacket;
}

/*
 * onData with retry
 */
template<class T1> void
Consumer<T1>::onData_Next(Data &data,uint32_t Seg_End) {
    uint32_t    Seg_Num,Pos,Len;
    Seg_Num = static_cast<uint32_t>(data.getName().get(-1).toSegment());
    Pos = Seg_Num * m_Size;
    if (m_FD != -1) {
        Len = writeFileData(m_FD,
                            Pos,
                            TOCCHARP(data.getContent().value()),
                            data.getContent().value_size());
    } else {
        Len = data.getContent().value_size();
    }
    CountSize += Len;
    SchedulePacket(Seg_Num + 1,Seg_End);
}

/*
 * onData without retry
 */
template<class T1> void
Consumer<T1>::onData(Data &data) {
    uint32_t    Seg_Num,Pos,Len;
    Seg_Num = static_cast<uint32_t>(data.getName().get(-1).toSegment());
    Pos = Seg_Num * m_Size;
    if (m_FD != -1){
        Len = writeFileData(m_FD,
                            Pos,
                            TOCCHARP(data.getContent().value()),
                            data.getContent().value_size());
    } else {
        Len = data.getContent().value_size();
    }
    CountSize += Len;
}

/*
 * onTimeOut with retry
 */
template<class T1> void
Consumer<T1>::onTimeOut_Retry(const Interest &interest,uint32_t Seg_End) {
    m_Face.expressInterest(interest,
                            bind(&Consumer::onData_Next,this,_2,Seg_End),
                            bind(&Consumer::onTimeOut_Retry,this,_1,Seg_End));
}

/*
 * onTimeOut without retry
 */
template<class T1> void
Consumer<T1>::onTimeOut_Noretry(const Interest &interest) {
    CountSize = 0;
}

}//end namespace ndn

#endif
