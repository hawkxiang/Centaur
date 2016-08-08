#include "App.hpp"
#include "Pipeline.hpp"

void Req_Info(const string InfoPrefix,
                const string InfoName,
                Info_Data *TempData,
                uint32_t Logout){
    pthread_t   tid;

    tid = pthread_self();

    log_time("[thread 0x%lx] REQ Start Info Data",tid);
    if (Logout == 1)
        log_App("[thread 0x%lx] Info Interest out",tid);
    ndn::Consumer<Info_Data> *CInfo =
        new ndn::Consumer<Info_Data>(InfoPrefix,InfoName);
    CInfo->opt_Retry_Data(0,1);
    CInfo->getData(TempData);
    CInfo->finish();
    free(CInfo);
    if (Logout == 1)
        log_App("[thread 0x%lx] Info Data in",tid);
    log_time("[thread 0x%lx] REQ End Info Data",tid);
    return;
}

void App_Data_Req_Pipeline(const string name,const string path,uint32_t Logout){
    pthread_t   tid;
    Info_Data   TempData;
    string      InfoName;
    string      InfoPrefix,DataPrefix;
    uint32_t    TempSeg;

    tid = pthread_self();
    InfoPrefix = App_Prefix(APP_PREFIX,INFO_PREFIX);
    DataPrefix = App_Prefix(APP_PREFIX,DATA_PREFIX);
    InfoName = name + "Info";
    log_time("[thread 0x%lx] Start Require Data: %s",tid,name.c_str());

    Req_Info(InfoPrefix,InfoName,&TempData,Logout);

    TempSeg = (TempData.size_Data - 1) / DATA_K + 1;

    log_debug("[thread 0x%lx] IP: %s Port: %d",
            tid,
            TempData.remote_IP.Address,
            TempData.remote_IP.Port);
    log_debug("[thread 0x%lx] Size: %ld Seg: %ld",
            tid,
            TempData.size_Data,
            TempSeg);

    NDNIP_Req *NDNIP_Pipeline =
        new NDNIP_Req(DataPrefix,TempData.remote_IP,name,path,TempSeg,tid,Logout);
    NDNIP_Pipeline->setLifetime(PIPELINETIME);
    log_time("[thread 0x%lx] REQ Start File Data",tid);
    for(uint32_t Seg=0;Seg<TempSeg;Seg++){
        NDNIP_Pipeline->NDN_Run(Seg,0);
    }
    NDNIP_Pipeline->finish();
    NDNIP_Pipeline->PipelineCount();
    free(NDNIP_Pipeline);
    log_time("[thread 0x%lx] REQ End File Data",tid);

    log_time("[thread 0x%lx] Stop Require Data: %s",tid,name.c_str());
    return;
}
