#include "App.hpp"

void App_Data_Req_NDN(const string name,const string path){
    Info_Data   TempData;
    pthread_t   tid;
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

    TempSeg = (TempData.size_Data - 1) / DATA_K + 1;
    ndn::Consumer<Data_K> *CData =
        new ndn::Consumer<Data_K>(DataPrefix,name,path,TempSeg);
    log_time("[thread 0x%lx] REQ Start File Data",tid);
    CData->opt_Retry_Data(0,TempSeg);
    CData->finish();
    free(CData);
    log_time("[thread 0x%lx] REQ End File Data",tid);

    log_time("[thread 0x%lx] End Require Data: %s",tid,name.c_str());
    return;
}
