#include "../App.hpp"

int main(int argc,char* argv[]){
    if(argc != 4){
        printf("<Usage>: COMMAND data_name data_path(absolute) Sleeptime(ms)\n");
        exit(EXIT_FAILURE);
    }

    struct msg_st   data;
    int             msgid = -1;
    msgid = msgget(MQ_KEY1,0666 | IPC_CREAT);

    if (msgid == -1){
        printf("APP ADD_DATA msgget() failed with error: %d\n",errno);
        exit(EXIT_FAILURE);
    }

    data.m_type = 1;
    data.m_command = APP_ADDDATA;
    strcpy(data.m_name,argv[1]);
    strcpy(data.m_path,argv[2]);
    data.m_args = atoi(argv[3]);

    if (msgsnd(msgid,(void*)&data,MSGSIZE,0) == -1){
        printf("APP ADD_DATA msgsnd() failed\n");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}
