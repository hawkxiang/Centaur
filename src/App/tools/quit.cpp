#include "../App.hpp"

int main(int argc,char* argv[]){
    if (argc != 1){
        printf("<Usage>: COMMAND\n");
        exit(EXIT_FAILURE);
    }

    struct msg_st   data;
    int             msgid = -1;
    msgid = msgget(MQ_KEY1, 0666 | IPC_CREAT);

    if(msgid == -1){
        printf("APP QUIT msgget() failed with error: %d\n",errno);
        exit(EXIT_FAILURE);
    }

    data.m_type = 1;
    data.m_command = APP_QUIT;

    if (msgsnd(msgid,(void*)&data,MSGSIZE,0) == -1){
        printf("APP QUIT msgsnd() failed\n");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}
