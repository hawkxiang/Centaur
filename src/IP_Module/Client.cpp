/*
 * Complete IP Client Module.
 */
#ifndef _IP_MODULE_CLIENT_CPP_
#include "Client.hpp"

#else

namespace ip {

/*
 * Conncet_retry
 * TCP Conncet retry when connect failed
 * bind function: socket() and connect()
 */
static inline int
connect_retry(int domain,int type,int protocol,
                const struct sockaddr *addr,socklen_t alen) {
    int     numsec,fd;
    for (numsec = 1; numsec <= MAXSLEEP; numsec <<= 1) {
        if ((fd = socket(domain,type,protocol)) < 0) {
            log_err("connect_rerty(): socket() error");
            return (-1);
        }
        if (connect(fd,addr,alen) == 0) {
            return (fd);
        }
        close(fd);
        if (numsec <= (MAXSLEEP >> 1))
            sleep(numsec);
    }
    return (-1);
}

/*
 * initialize server address(IPv4 address and port)
 */
template<class T1> void
Client<T1>::init_serv_addr(const IP_v4_Address Serv_Addr) {
    memset(&m_serv_adr,0,sizeof(m_serv_adr));
    m_serv_adr.sin_family = AF_INET;
    m_serv_adr.sin_addr.s_addr = inet_addr(Serv_Addr.Address);
    m_serv_adr.sin_port = htons(Serv_Addr.Port);
}

/*
 * Receive data to file(fd) int position(pos).
 * Knowing total segment and segmenst size.
 * @param  : {int}          fd
 * @param  : {uint32_t}     pos
 * @return : {uint32_t}     receive size
 */
template<class T1> uint32_t
Client<T1>::receiveData(int fd,uint32_t Pos) {
    T1          Temp;
    uint32_t    TempLen,Size,Total,Fdpos;
    //receive data size
    write(fd,&Pos,sizeof(Pos));
    if (Pos == m_Seg - 1) {
        //when data segment in the end.
        read(fd,&Size,sizeof(Size));
    } else {
        Size = m_Size;
    }
    //receive data content
    Total = 0;
    while (Total < Size) {
        TempLen = read(fd,TOCHARP(&Temp)+Total,Size - Total);
        Total += TempLen;
    }
    Fdpos = Pos * m_Size;
    return (writeFileData(m_FD,Fdpos,TOCHARP(&Temp),Total));
}

/*
 * close Client Req.
 * close file description and delete temp file.
 */
template<class T1> void
Client<T1>::closeReq() {
    if (m_Type == INFO_DATA) {
        delete_file(m_FD);
    } else {
        close_file(m_FD);
    }
}

/*
 * complete TCP connect.
 * find Data and initialize data information.
 */
template<class T1> int
Client<T1>::run_TCP() {
    int32_t     TempRet;
    uint32_t    Seg;
    if (TCP_Count == 1) {
        log_err("run_TCP() Connected twice.");
        return (-1);
    }
    m_TCP_sock = connect_retry(PF_INET,
                                SOCK_STREAM,
                                0,
                                (struct sockaddr*)&m_serv_adr,
                                sizeof(m_serv_adr));
    if (m_TCP_sock < 0) {
        log_err("run_TCP() Connect failed.");
        return (-1);
    } else {
        log_debug("Connected...");
    }
    //check data
    write(m_TCP_sock,&m_NameLen,sizeof(m_NameLen));
    write(m_TCP_sock,m_DName.c_str(),m_NameLen);
    read(m_TCP_sock,&TempRet,sizeof(TempRet));
    if (TempRet < 0) {
        log_err("run_TCP() Server can't own Data.");
        finish_TCP();
        return (-1);
    }
    log_debug("Server own Data.");
    write(m_TCP_sock,&m_Size,sizeof(m_Size));
    write(m_TCP_sock,&m_Seg,sizeof(m_Seg));
    read(m_TCP_sock,&Seg,sizeof(Seg));
    TCP_Count = 1;
    return (0);
}

/*
 * Through TCP to receive data.
 * @param   {uint32_t}  Seg_Pos
 * @param   {uint32_t}  Seg_Len
 * @return  {uint32_t}  Total_Size
 */
template<class T1> uint32_t
Client<T1>::opt_TCP(uint32_t Seg_Pos,uint32_t Seg_Len) {
    uint32_t     Cur_Pos,Total;
    pthread_mutex_lock(&TCP_Lock);
    Cur_Pos = Seg_Pos;
    Total = 0;
    while ((Cur_Pos < m_Seg) && (Seg_Len > 0)) {
        Total += receiveData(m_TCP_sock,Cur_Pos);
        Cur_Pos++;
        Seg_Len--;
    }
    pthread_mutex_unlock(&TCP_Lock);
    return (Total);
}

/*
 * finish TCP connect and close REQ
 */
template<class T1> void
Client<T1>::finish_TCP() {
    close(m_TCP_sock);
    TCP_Count = 0;
    Client_Flag = -1;
    this->closeReq();
}

template<class T1> uint32_t
Client<T1>::getData(T1 *Data) {
    if (Client_Flag == -1) {
        log_err("Client getData() can't get Data.");
        return (-1);
    }
    if (m_Type == FILE_DATA) {
        log_err("Client getData() it's file data.");
        return (-1);
    }
    return ((readFileData(m_FD,0,TOCHARP(Data),m_Size)));
}

}//end namespace ip
#endif
