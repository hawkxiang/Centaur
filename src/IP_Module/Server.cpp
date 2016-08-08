/*
 * Complete IP Server Module.
 */
#include "Server.hpp"

namespace ip{

/*
 * Class Server public function.
 * get_Dname()  return Dname(string)
 * get_FD()     return m_FD(int)
 * finish()     close m_FD
 */
string
Server::get_Dname() {
    return (m_Dname);
}

int
Server::get_FD() {
    return (m_FD);
}

void
Server::finish() {
    if (m_Type == INFO_DATA) {
        delete_file(m_FD);
    } else {
        close_file(m_FD);
    }
}
void
Server::setmTimes(uint32_t mTimes){
    m_Times = mTimes * 1000;
}
uint32_t
Server::getmTimes(){
    return m_Times;
}

/*
 * Server pool module.
 */
struct sockaddr_in      pool_serv_adr;
struct sockaddr_in      local_serv_adr;
int                     TCP_serv_sock;
int                     TCP_clnt_sock;
pthread_mutex_t         Serv_lock = PTHREAD_MUTEX_INITIALIZER;

std::map<string,Server*>    Serv_Map;
std::map<string,int>        Serv_Count;

/* main thread quit number */
const uint32_t          Quit_Num = 65535;

void        _run_TCP_Server_Class(int clnt_sock,Server* Ser);
void        TCP_Server_thread();
static int  add_TCP_Server_pool(Server *Ser);

/*
 * server class thread.
 * serve one client request.
 */
void
_run_TCP_Server_Class(int clnt_sock,Server* Ser) {
    int         FD;
    uint32_t    Pos,Dsize,Fdpos,Size,Seg;
    uint32_t    Times;
    uint8_t     *Temp;
    string      name;
    name = Ser->get_Dname();
    FD = Ser->get_FD();
    Times = Ser->getmTimes();
    read(clnt_sock,&Size,sizeof(Size));
    read(clnt_sock,&Seg,sizeof(Seg));
    write(clnt_sock,&Seg,sizeof(Seg));
    Temp = (uint8_t*)calloc(Size,sizeof(uint8_t));
    while (read(clnt_sock,&Pos,sizeof(Pos)) != 0) {
        Fdpos = Pos * Size;
        Dsize = readFileData(FD,Fdpos,TOCHARP(Temp),Size);
        usleep(Times);
        if (Pos == Seg - 1) {
            write(clnt_sock,&Dsize,sizeof(Dsize));
        }
        write(clnt_sock,TOCHARP(Temp),Dsize);
    }
    /*
     * delete server form server pool when delete flag on
     * delete flag : serv_Count == 0 && Serv_Map delete
     */
    pthread_mutex_lock(&Serv_lock);
    close(clnt_sock);
    free(Temp);
    Serv_Count[name]--;
    if ((Serv_Count[name] == 0) && (Serv_Map.find(name) == Serv_Map.end())) {
        Serv_Count.erase(name);
        Ser->finish();
        free(Ser);
    }
    pthread_mutex_unlock(&Serv_lock);
}

/*
 * initialize server pool
 * pool_serv_adr & local serv_adr
 */
void
init_Server_pool(int Port) {
    memset(&pool_serv_adr,0,sizeof(pool_serv_adr));
    pool_serv_adr.sin_family = AF_INET;
    pool_serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
    pool_serv_adr.sin_port = htons(Port);
    memset(&local_serv_adr,0,sizeof(local_serv_adr));
    local_serv_adr.sin_family = AF_INET;
    local_serv_adr.sin_addr.s_addr = inet_addr("127.0.0.1");
    local_serv_adr.sin_port = htons(Port);
}

/*
 * configure server socket and run main pthread.
 */
int
run_TCP_Server(int backlog) {
    TCP_serv_sock = socket(PF_INET,SOCK_STREAM,0);
    if (TCP_serv_sock < 0) {
        log_err("run_TCP_server() socket() error.");
        return (-1);
    }
    if (bind(TCP_serv_sock,
            (struct sockaddr*)&pool_serv_adr,
            sizeof(pool_serv_adr)) == -1) {
        log_err("run_TCP_server() bind() error.");
        close(TCP_serv_sock);
        return (-1);
    }
    if (listen(TCP_serv_sock,backlog) == -1) {
        log_err("run_TCP_server() listen() error.");
        close(TCP_serv_sock);
        return (-1);
    }
    boost::thread(boost::bind(TCP_Server_thread));
    return 0;
}

/*
 * the main thread
 * listen client request.
 */
void
TCP_Server_thread() {
    socklen_t           clnt_adr_size;
    struct sockaddr_in  clnt_adr;
    uint32_t            TempNameLen;
    int32_t             TempRet;
    char                *TempName;
    while (true) {
        clnt_adr_size = sizeof(clnt_adr);
        TCP_clnt_sock = accept(TCP_serv_sock,
                                (struct sockaddr*)&clnt_adr,
                                &clnt_adr_size);
        //Check Data.
        read(TCP_clnt_sock,&TempNameLen,sizeof(TempNameLen));
        if (TempNameLen == 65535) {
            break;
        }
        TempName = (char*)calloc(TempNameLen+1,sizeof(char));
        read(TCP_clnt_sock,TempName,TempNameLen);
        TempName[TempNameLen] = 0;
        pthread_mutex_lock(&Serv_lock);
        auto map_it = Serv_Map.find(TempName);
        if (map_it == Serv_Map.end()) {
            TempRet = 0;
        } else {
            TempRet = 1;
        }
        write(TCP_clnt_sock,&TempRet,sizeof(TempRet));
        // Run Data thread
        if (TempRet == 1) {
            Serv_Count[TempName]++;
            boost::thread(boost::bind(_run_TCP_Server_Class,
                                        TCP_clnt_sock,
                                        map_it->second));
        }
        pthread_mutex_unlock(&Serv_lock);
        free(TempName);
    }
    log_debug("Server Pool ends");
    close(TCP_serv_sock);
}

/*
 * close TCP server
 * send Quit Num to local server.
 */
void
close_TCP_Server() {
    int         sock;
    Server*     Ser;
    string      name;
    sock = socket(PF_INET,SOCK_STREAM,0);
    connect(sock,(struct sockaddr*)&local_serv_adr,sizeof(local_serv_adr));
    write(sock,&Quit_Num,sizeof(Quit_Num));
    close(sock);

    //Delete All TCP Server
    log_debug("delete all TCP Servers");
    pthread_mutex_lock(&Serv_lock);
    while (Serv_Map.begin() != Serv_Map.end()){
        name = (Serv_Map.begin())->first;
        Ser = (Serv_Map.begin())->second;
        log_debug("close_TCP_Server(): delete Server Name %s",name.c_str());
        Serv_Map.erase(name);
        if (Serv_Count[name] == 0) {
            Serv_Count.erase(name);
            Ser->finish();
            free(Ser);
        }
    }
    pthread_mutex_unlock(&Serv_lock);

    while (Serv_Count.begin() != Serv_Count.end()) {
        log_debug("close TCP Server Failed......");
        boost::this_thread::yield();
    }
}

/*
 * add TCP Server with info data.
 */
int
add_TCP_Server(const string name,const void* Data,const uint32_t Size) {
    Server *Ser = new Server(name,Data,Size);
    if (add_TCP_Server_pool(Ser) < 0) {
        free(Ser);
        return -1;
    }
    return 0;
}

/*
 * add TCP server with file data.
 */
int
add_TCP_Server(const string name,const string Path) {
    Server *Ser = new Server(name,Path);
    if (add_TCP_Server_pool(Ser) < 0) {
        free(Ser);
        return -1;
    }
    return 0;
}

/*
 * add TCP server with sleep time.
 */
int
add_TCP_Server(const string name,const string Path,const uint32_t mTimes){
    Server *Ser = new Server(name,Path);
    if (add_TCP_Server_pool(Ser) < 0) {
        free(Ser);
        return -1;
    }
    Ser->setmTimes(mTimes);
    return 0;
}

/*
 * add TCP server to Server pool.
 */
static int
add_TCP_Server_pool(Server *Ser) {
    pthread_mutex_lock(&Serv_lock);
    string  name;
    name = Ser->get_Dname();
    if (Serv_Map.find(name) != Serv_Map.end()) {
        log_err("add_TCP_Server_pool() server %s Data name conflict.",name.c_str());
        pthread_mutex_unlock(&Serv_lock);
        return (-1);
    }
    Serv_Count.insert(std::make_pair(name,0));
    Serv_Map.insert(std::make_pair(name,Ser));
    pthread_mutex_unlock(&Serv_lock);
    return (0);
}

/*
 * delete TCP server from Server pool.
 */
int
del_TCP_Server(const string name) {
    pthread_mutex_lock(&Serv_lock);
    Server*     Ser;
    if (Serv_Map.find(name) == Serv_Map.end()) {
        log_err("del_TCP_Server() can't find server %s.",name.c_str());
        pthread_mutex_unlock(&Serv_lock);
        return (-1);
    }
    Ser = Serv_Map[name];
    Serv_Map.erase(name);
    if (Serv_Count[name] == 0) {
        Serv_Count.erase(name);
        Ser->finish();
        free(Ser);
    }
    pthread_mutex_unlock(&Serv_lock);
    return (0);
}

}//end namespace ip
