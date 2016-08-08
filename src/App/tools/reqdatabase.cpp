#include "../App.hpp"

int main(int argc,char* argv[]){
    if(argc != 3){
        printf("<Usage>: COMMAND data_name data_path(absolute)\n");
        exit(EXIT_FAILURE);
    }

    struct msg_st   data;
    int             msgid = -1;
    msgid = msgget(MQ_KEY1,0666 | IPC_CREAT);

    if (msgid == -1){
        printf("APP REQ_DATA_B msgget() failed with error: %d\n",errno);
        exit(EXIT_FAILURE);
    }

    data.m_type = 1;
    data.m_command = APP_REQDATA_B;
    strcpy(data.m_name,argv[1]);
    strcpy(data.m_path,argv[2]);

    if (msgsnd(msgid,(void*)&data,MSGSIZE,0) == -1){
        printf("APP REQ_DATA_B msgsnd() failed\n");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}
