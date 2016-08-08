#include "Pipeline.hpp"

void App_Data_Add(const string name,const string path,
                    const struct IP_v4_Address local_IP,
                    const uint32_t mTimes){
    Info_Data   TempData;
    string      InfoName;
    string      InfoPrefix,DataPrefix;

    InfoPrefix = App_Prefix(APP_PREFIX,INFO_PREFIX);
    DataPrefix = App_Prefix(APP_PREFIX,DATA_PREFIX);
    InfoName = name + "Info";
    TempData.remote_IP = local_IP;
    log_debug("%s %d",TempData.remote_IP.Address,TempData.remote_IP.Port);
    boost::filesystem::path Path(path.c_str());
    TempData.size_Data = boost::filesystem::file_size(Path);

    ndn::add_NDN_Producer(InfoPrefix,InfoName,&TempData,sizeof(TempData));

    ip::add_TCP_Server(name,path,mTimes);
    ndn::add_NDN_Producer(DataPrefix,name,path);
    log_debug("App Data Add success.");
    return;
}

void App_Data_Del(const string name){
    string      InfoName;
    string      InfoPrefix,DataPrefix;

    InfoPrefix = App_Prefix(APP_PREFIX,INFO_PREFIX);
    DataPrefix = App_Prefix(APP_PREFIX,DATA_PREFIX);
    InfoName = name + "Info";

    ndn::del_NDN_Producer(InfoPrefix,InfoName);

    ip::del_TCP_Server(name);
    ndn::del_NDN_Producer(DataPrefix,name);
    log_debug("App Data Del success.");
    return;
}

