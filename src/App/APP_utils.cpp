#include "App.hpp"

string App_Prefix(const string Prefix1,
                    const string Prefix2){
    return Prefix1+Prefix2;
}

void App_Proxy(const string name){
    string      InfoName;
    string      InfoPrefix;

    InfoPrefix = App_Prefix(APP_PREFIX,INFO_PREFIX);
    InfoName = name + "Info";

    ndn::add_NDN_Producer(InfoPrefix,InfoName,MQ_KEY1,APP_RES,name);
    log_debug("APP add Proxy success.");
}

void App_Proxy_res(const string name,
                    const struct IP_v4_Address local_IP){
    string      InfoName;
    string      TempPath;
    string      InfoPrefix,DataPrefix;
    Info_Data   TempData;
    uint32_t    TempSeg;

    log_debug("APP Response Proxy");
    InfoPrefix = App_Prefix(APP_PREFIX,INFO_PREFIX);
    DataPrefix = App_Prefix(APP_PREFIX,DATA_PREFIX);
    InfoName = name + "Info";

    /* Delete App Proxy*/
    ndn::del_NDN_Producer(InfoPrefix,InfoName);
    /* Require File Data with IP and NDN*/
    ndn::Consumer<Info_Data> *CInfo =
        new ndn::Consumer<Info_Data>(InfoPrefix,InfoName);
    CInfo->opt_Retry_Data(0,1);
    CInfo->getData(&TempData);
    CInfo->finish();
    free(CInfo);

    TempPath = DATAFILEDIR;
    TempPath = TempPath +"/"+APP_PREFIX+name;
    TempSeg = ((TempData.size_Data-1) / DATA_K) + 1;

    ip::Client<Data_K> *CData =
        new ip::Client<Data_K>(TempData.remote_IP,name,TempPath,TempSeg);
    CData->run_TCP();
    CData->opt_TCP(0,TempSeg);
    CData->finish_TCP();
    free(CData);

    /* Add File Data Producer and Server */
    TempData.remote_IP = local_IP;
    ndn::add_NDN_Producer(InfoPrefix,InfoName,&TempData,sizeof(TempData));
    ip::add_TCP_Server(name,TempPath);
    ndn::add_NDN_Producer(DataPrefix,name,TempPath);

    log_debug("APP Response Proxy success.");
}

uint32_t* setSegList(uint32_t SegNum,uint32_t percent){
    uint32_t    *List;
    uint32_t    Temp;
    uint32_t    i,j,index;
    uint32_t    ret;

    Temp = (SegNum * percent)/100;
    List = (uint32_t*)malloc(sizeof(uint32_t)*(Temp+1));
    List[0] = Temp;
    index = (SegNum/Temp)-1;
    ret = SegNum - Temp;
    j = 0;
    for (i=1;i<=Temp;i++){
        List[i] = j;
        if (ret >= index){
            j += index;
            ret-=index;
        }else{
            j += ret;
            ret=0;
        }
        j++;
    }
    return List;
}

void App_Prefetch(const string name,uint32_t percent){
    string      InfoName;
    string      InfoPrefix,DataPrefix;
    Info_Data   TempData;
    uint32_t    TempSeg;
    uint32_t    *SegList;
    log_debug("APP prefetch file data: %s",name.c_str());
    InfoPrefix = App_Prefix(APP_PREFIX,INFO_PREFIX);
    DataPrefix = App_Prefix(APP_PREFIX,DATA_PREFIX);
    InfoName = name + "Info";
    ndn::Consumer<Info_Data> *CInfo =
        new ndn::Consumer<Info_Data>(InfoPrefix,InfoName);
    CInfo->opt_Retry_Data(0,1);
    CInfo->getData(&TempData);
    CInfo->finish();
    free(CInfo);

    TempSeg = ((TempData.size_Data - 1) / DATA_K) + 1;
    SegList = setSegList(TempSeg,percent);

    log_debug("App prefetch Seg Number: %ld/%ld",SegList[0],TempSeg);
    ndn::Consumer<Data_K> *CData =
        new ndn::Consumer<Data_K>(DataPrefix,name,TempSeg);
    CData->setLifetime_mseconds(PREFETCHTIME);
    while(SegList[0] != 0){
        CData->opt_Retry_Data(SegList[SegList[0]],1);
        SegList[0]--;
    }
    CData->finish();
    free(CData);
    free(SegList);
    log_debug("APP prefetch file data: %s success.",name.c_str());
}
