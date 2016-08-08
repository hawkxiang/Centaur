#include "Pipeline.hpp"

/*
 * Add Jobs to IP Pipeline queue.
 */
void
NDNIP_Req::Add_Jobs(uint32_t Seg){
    /*struct IP_Jobs  *Jobs;
    Jobs = (struct IP_Jobs*)malloc(sizeof(struct IP_Jobs));
    Jobs->Seg = Seg;
    Jobs->next = NULL;*/
    pthread_mutex_lock(&IP_lock);
    IP_queue.Count+=1;
    IP_queue.Jobs[IP_queue.tail] = Seg;
    IP_queue.tail = (IP_queue.tail + 1) and (MAXJOBS-1);
    /*
    if (IP_queue.tail == NULL){
        IP_queue.head = Jobs;
        IP_queue.tail = Jobs;
    } else {
        IP_queue.tail->next = Jobs;
        IP_queue.tail = Jobs;
    }
    */
    pthread_mutex_unlock(&IP_lock);
}

void
NDNIP_Req::runIP_Pipeline(){
    //struct IP_Jobs  *Jobs;
    uint32_t        Jobs;
    struct timeval  T1,T2;
    uint64_t        Time;
    log_debug("[thread 0x%lx] IP Pipeline Running.",NI_tid);
    while (IP_running){
        while (IP_queue.Count != 0){
            pthread_mutex_lock(&IP_lock);
            IP_queue.Count-=1;
            Jobs = IP_queue.Jobs[IP_queue.head];
            IP_queue.head = (IP_queue.head +1) and (MAXJOBS-1);
            /*Jobs = IP_queue.head;
            IP_queue.head = IP_queue.head->next;
            if (IP_queue.head == NULL){
                IP_queue.tail = NULL;
            }
            */
            pthread_mutex_unlock(&IP_lock);
            if(NI_Logout == 1)
                log_App("[thread 0x%lx] Data Seg %ld TCP/IP out",NI_tid,Jobs);
            gettimeofday(&T1,NULL);
            IData->opt_TCP(Jobs,1);
            gettimeofday(&T2,NULL);
            Time = (T2.tv_sec - T1.tv_sec) * 1000
                + (T2.tv_usec)/1000
                - (T1.tv_usec)/1000;
            log_debug("Times: %ld",Time);
            if (NI_Logout == 1)
                log_App("[thread 0x%lx] Data Seg %ld TCP/IP in",NI_tid,Jobs);
            //free(Jobs);
            boost::this_thread::yield();
        }
        boost::this_thread::yield();
    }
    IData->finish_TCP();
    log_debug("[thread 0x%lx] IP Pipeline Stop...",NI_tid);
}

void
NDNIP_Req::setLifetime(uint32_t Times){
    NData->setLifetime_mseconds(Times);
}

void
NDNIP_Req::NDN_Run(uint32_t Seg,uint32_t Times){
    size_t  Ret;

    TCount+=1;
    NCount+=1;
    if (Times != 0){
        NData->setLifetime_mseconds(Times);
    }
    if(NI_Logout == 1)
        log_App("[thread 0x%lx] Data Seg %ld Interest out",NI_tid,Seg);
    Ret = NData->opt_Noretry_Data(Seg);
    if (Ret != 0) {
        if (NI_Logout == 1)
            log_App("[thread 0x%lx] Data Seg %ld Data in",NI_tid,Seg);
    } else {
        if (NI_Logout == 1)
            log_App("[thread 0x%lx] Data Seg %ld Timeout",NI_tid,Seg);
        log_debug("[thread 0x%lx] NDN Timeout Seg: %ld.",NI_tid,Seg);
        NCount-=1;
        Add_Jobs(Seg);
    }
}

void
NDNIP_Req::finish(){
    IP_running = 0;
    IP_Pipeline.join_all();
    NData->finish();
    free(NData);
    free(IData);
    return;
}

void
NDNIP_Req::PipelineCount(){
    log_info("[thrrad 0x%lx] Pipeline Total Seg: %ld, NDN Seg: %ld, IP Seg: %ld",
                NI_tid,TCount,NCount,TCount-NCount);
}
