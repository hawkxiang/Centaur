/*
 * Complete NDN Producer Module.
 */
#include "Producer.hpp"

namespace ndn {

string
Producer::get_Name(){
    return m_Name;
}
/*
 * receive interest packet.
 * create data packet.
 * return data packet.
 */
void
Producer::onInterest(const Interest &interest) {
    uint32_t    Pos,len;
    uint32_t    seg_num,seg_size;
    uint8_t     *Temp;
    shared_ptr<Data> dataPacket = make_shared<Data>(interest.getName());
    seg_num = static_cast<uint32_t>(interest.getName().get(-1).toSegment());
    seg_size = static_cast<uint32_t>(interest.getName().get(-2).toNumber());
    Pos = seg_num * seg_size;
    Temp = (uint8_t*)calloc(seg_size,sizeof(uint8_t));
    len = readFileData(m_FD,
                        Pos,
                        TOCHARP(Temp),
                        seg_size);
    dataPacket->setContent(TOCCHARP(Temp),len);
    if (m_Type == INFO_DATA){
        dataPacket->setFreshnessPeriod(time::milliseconds(5));
    }
    m_KeyChain.sign(*dataPacket);
    m_Face.put(*dataPacket);
    free(Temp);
    m_Count+=1;
}

void
Producer::onProxy(const Interest &interest){
    pthread_mutex_lock(&proxy_lock);
    if (running == 1){
        pthread_mutex_unlock(&proxy_lock);
        return;
    }
    running = 1;
    if (msgsnd(msgid,proxy_msg,MSGSIZE,0) == -1) {
        log_err("Proxy Producer msgsnd() failed\n");
        pthread_mutex_unlock(&proxy_lock);
        exit(EXIT_FAILURE);
    }
    pthread_mutex_unlock(&proxy_lock);
}

/*
 * Producer registerfailed.
 */
void
Producer::onRegisterFailed(const Name &prefix,const string &reason) {
    log_info("Prefix Registeration Failure.");
    log_info("Reason = %s",reason.c_str());
    this->finish();
}

/*
 * run Producer
 */
void
Producer::run() {
    if (m_Type == PROXY_TEMP){
        m_Face.setInterestFilter(Name(m_Name),
                                bind(&Producer::onProxy,this,_2),
                                RegisterPrefixSuccessCallback(),
                                bind(&Producer::onRegisterFailed,this,_1,_2));
    } else {
        m_Face.setInterestFilter(Name(m_Name),
                                bind(&Producer::onInterest,this,_2),
                                RegisterPrefixSuccessCallback(),
                                bind(&Producer::onRegisterFailed,this,_1,_2));
    }
    m_Face.processEvents();
}

/*
 * close producer and close(delete) file.
 */
void
Producer::finish() {
    m_Face.shutdown();
    if (m_Type != PROXY_TEMP){
        log_time("Producer %s onInterests %d.",m_Name.c_str(),m_Count);
    }
    if (m_Type == INFO_DATA) {
        delete_file(m_FD);
    } else {
        close_file(m_FD);
    }
}

}//end namespace ndn

