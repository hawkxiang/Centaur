#include "App.hpp"

static struct IP_v4_Address local_IP;

/*
 * Initialize App
 */
int init_App(int argc,char* argv[]){
    int     i;
    int     Backlog = 50;
    int     msgid = -1;

    log_time("App Initialize");

    msgid = msgget(MQ_KEY1,0666 | IPC_CREAT);
    if (msgid == -1){
        log_err("APP msgget() failed with errno: %d",errno);
        exit(EXIT_FAILURE);
    }

    set_ver_debug(0);
    for(i=1;i<argc;i++)
        if (strcmp(argv[i],"-p")==0){
            local_IP.Port = atoi(argv[i+1]);
        } else
        if (strcmp(argv[i],"-q")==0){
            Backlog = atoi(argv[i+1]);
        } else
        if (strcmp(argv[i],"-f")==0){
            set_file_out(argv[i+1]);
        } else
        if (strcmp(argv[i],"-d")==0){
            set_ver_debug(1);
        } else
        if (strcmp(argv[i],"-l")==0){
            set_App_out(argv[i+1]);
        }

    if (__SYSTEM__==1)
        tools::getLocalIP_1(local_IP.Address);
    else
        tools::getLocalIP_2(local_IP.Address);

    init_Storage_Module();

    ip::init_Server_pool(local_IP.Port);
    ip::run_TCP_Server(Backlog);

    log_info("Local Server Address: %s",local_IP.Address);
    log_info("Local Server Port: %d",local_IP.Port);
    log_info("Local Server Listen backlog: %d",Backlog);

    return msgid;
}

int main(int argc,char* argv[]){

    log_time("App startup");
    int             msgid = init_App(argc,argv);
    long            msgtype = 0;
    int             running = 1;
    struct msg_st   data;

    while(running) {
        if(msgrcv(msgid,(void*)&data,MSGSIZE,msgtype,0) == -1) {
            log_err("APP msgrcv() failed with errno: %d",errno);
            exit(EXIT_FAILURE);
        }
        if (data.m_command == APP_QUIT) {
            running = 0;
            log_time("App prepare Quit");
        } else
        if (data.m_command == APP_PROXY){
            App_Proxy((string)(data.m_name));
        } else
        if (data.m_command == APP_RES){
            boost::thread(boost::bind(App_Proxy_res,
                                    (string)(data.m_name),
                                    local_IP));
        } else
        if (data.m_command == APP_PREFETCH){
            boost::thread(boost::bind(App_Prefetch,
                                    (string)(data.m_name),
                                    data.m_args));
        } else
        if (data.m_command == APP_ADDDATA){
            App_Data_Add(data.m_name,data.m_path,local_IP,data.m_args);
        } else
        if (data.m_command == APP_DELDATA){
            App_Data_Del(data.m_name);
        } else
        if (data.m_command == APP_REQDATA_B){
            boost::thread(boost::bind(App_Data_Req_Basic,
                            (string)(data.m_name),
                            (string)(data.m_path)));
            boost::this_thread::yield();
        } else
        if (data.m_command == APP_REQDATA_P){
            boost::thread(boost::bind(App_Data_Req_Pipeline,
                            (string)(data.m_name),
                            (string)(data.m_path),
                            data.m_args));
            boost::this_thread::yield();
        } else
        if (data.m_command == APP_REQDATA_N){
            boost::thread(boost::bind(App_Data_Req_NDN,
                                (string)(data.m_name),
                                (string)(data.m_path)));
            boost::this_thread::yield();
        }
    }

    ip::close_TCP_Server();
    if (msgctl(msgid,IPC_RMID,0) == -1){
        log_err("APP msgctl(IPC_RMID) failed.");
        exit(EXIT_FAILURE);
    }
    log_time("App enddown");
}
