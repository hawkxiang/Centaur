#include "../App.hpp"

int main(int argc,char* argv[]){
    if(argc != 2){
        printf("<Usage>: COMMAND data_name\n");
        exit(EXIT_FAILURE);
    }

    struct msg_st   data;
    int             msgid = -1;
    msgid = msgget(MQ_KEY1,0666 | IPC_CREAT);

    if (msgid == -1){
        printf("APP PROXY msgget() failed with error: %d\n",errno);
        exit(EXIT_FAILURE);
    }

    data.m_type = 1;
    data.m_command = APP_PROXY;
    strcpy(data.m_name,argv[1]);

    if (msgsnd(msgid,(void*)&data,MSGSIZE,0) == -1){
        printf("APP PROXY msgsnd() failed\n");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}
