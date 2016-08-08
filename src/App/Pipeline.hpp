#ifndef _APP_PIPELINE_HPP_
#define _APP_PIPELINE_HPP_

#include "App.hpp"

const uint32_t MAXJOBS = (2<<15)-1;

struct IP_Jobs{
    struct IP_Jobs      *next;
    uint32_t            Seg;
};
struct IP_Jobs_queue{
    struct IP_Jobs      *head;
    struct IP_Jobs      *tail;
    uint32_t            Count;
};

struct IP_Jobs_queue2{
    uint32_t            head,tail;
    uint32_t            Jobs[MAXJOBS];
    uint32_t            Count;
};

class NDNIP_Req{
    public:
        explicit NDNIP_Req(const string DataPrefix,
                            const IP_v4_Address remoteIP,
                            const string name,const string path,
                            const uint32_t Seg,
                            const pthread_t tid,
                            const uint32_t Logout){
            NData = new ndn::Consumer<Data_K>(DataPrefix,name,path,Seg);
            IData = new ip::Client<Data_K>(remoteIP,name,path,Seg);
            NI_tid = tid;
            NI_Logout = Logout;
            NCount = 0;
            TCount = 0;
            IP_running = 1;
            IP_queue.head = 0;
            IP_queue.tail = 0;
            IP_queue.Count = 0;
            IData->run_TCP();
            log_debug("[thread 0x%lx] TCP/IP Connected...",NI_tid);
            IP_Pipeline.create_thread(boost::bind(&NDNIP_Req::runIP_Pipeline,this));
            log_debug("[thread 0x%lx] Init NDNIP Req File Pipeline success.",NI_tid);
        }
        void    setLifetime(uint32_t Times);
        void    NDN_Run(uint32_t Seg,uint32_t Times);
        void    finish();
        void    PipelineCount();

    private:
        void    Add_Jobs(uint32_t Seg);
        void    runIP_Pipeline();

    private:
        ndn::Consumer<Data_K>   *NData;
        ip::Client<Data_K>      *IData;

        pthread_t               NI_tid;
        uint32_t                NI_Logout;

        uint32_t                NCount,TCount;

        boost::thread_group     IP_Pipeline;
        IP_Jobs_queue2          IP_queue;
        uint32_t                IP_running;
        pthread_mutex_t         IP_lock = PTHREAD_MUTEX_INITIALIZER;
};

#endif//_APP_PIPELINE_HPP_
