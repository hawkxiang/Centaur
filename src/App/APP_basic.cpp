#include "App.hpp"

void App_Data_Req_Basic(const string name,const string path){
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
    log_time("[thread 0x%lx] REQ Start Info Data",tid);
    ndn::Consumer<Info_Data> *CInfo =
        new ndn::Consumer<Info_Data>(InfoPrefix,InfoName);
    CInfo->opt_Retry_Data(0,1);
    CInfo->getData(&TempData);
    CInfo->finish();
    free(CInfo);
    log_time("[thread 0x%lx] REQ End Info Data",tid);

    TempSeg = ((TempData.size_Data - 1) / DATA_K) + 1;
    log_debug("[thread 0x%lx] IP: %s Port %d",
            tid,
            TempData.remote_IP.Address,
            TempData.remote_IP.Port);
    log_debug("[thread 0x%lx] Size: %ld Seg: %ld",
            tid,
            TempData.size_Data,
            TempSeg);

    ip::Client<Data_K> *CData =
        new ip::Client<Data_K>(TempData.remote_IP,name,path,TempSeg);
    CData->run_TCP();
    log_time("[thread 0x%lx] REQ Start File Data",tid);
    CData->opt_TCP(0,TempSeg);
    CData->finish_TCP();
    free(CData);
    log_time("[thread 0x%lx] REQ End File Data",tid);

    log_time("[thread 0x%lx] Stop Require Data: %s",tid,name.c_str());
    return;
}
