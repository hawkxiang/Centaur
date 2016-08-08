#ifndef _APP_APP_HPP_
#define _APP_APP_HPP_

#include "../Common.hpp"
#include "../Storage_Module/Storage_Module.hpp"
#include "../IP_Module/Server.hpp"
#include "../NDN_Module/Producer.hpp"
#include "../IP_Module/Client.hpp"
#include "../NDN_Module/Consumer.hpp"
#include "../tools/Common.hpp"

#define APP_PREFIX "netlab/Centaur"
#define INFO_PREFIX "/INFO"
#define DATA_PREFIX "/DATA"


#define DATA_K DATA_8K
#define Data_K Data_8K

#define MQ_KEY1 1234L

#define APP_QUIT        0x01
#define APP_ADDDATA     0x02
#define APP_DELDATA     0x03
#define APP_PROXY       0x04
#define APP_RES         0x05
#define APP_PREFETCH    0x06
#define APP_REQDATA_B   0x11
#define APP_REQDATA_P   0x12
#define APP_REQDATA_N   0x13

#define PREFETCHTIME    1000
#define PIPELINETIME    1

struct Info_Data{
    uint32_t                size_Data;
    struct IP_v4_Address    remote_IP;
};

/* APP_utils.cpp */
string App_Prefix(const string Prefix1,const string Prefix2);
void App_Proxy(const string name);
void App_Proxy_res(const string name,
                    const struct IP_v4_Address local_IP);
void App_Prefetch(const string name,uint32_t percent);

/* APP_AddDel.cpp */
void App_Data_Add(const string name,const string path,
                    const struct IP_v4_Address local_IP,
                    const uint32_t mTimes);
void App_Data_Del(const string name);

/* APP_basic.cpp & APP_pipeline.cpp & APP_NDN.cpp */
void App_Data_Req_Basic(const string name,const string path);
void App_Data_Req_Pipeline(const string name,const string path,uint32_t Logout);
void App_Data_Req_NDN(const string name,const string path);

#endif//_APP_APP_HPP_
