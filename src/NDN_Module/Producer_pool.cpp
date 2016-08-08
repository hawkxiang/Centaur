#include "Producer.hpp"

namespace ndn{

pthread_mutex_t Pro_lock = PTHREAD_MUTEX_INITIALIZER;

std::map<string,Producer*> Pro_Map;

static int
add_NDN_Producer_pool(Producer* Pro){
    pthread_mutex_lock(&Pro_lock);
    string name;
    name = Pro->get_Name();
    if (Pro_Map.find(name) != Pro_Map.end()) {
        log_err("add_NDN_Producer_pool() producer %s Producer name conflict.",
                name.c_str());
        pthread_mutex_unlock(&Pro_lock);
        return (-1);
    }
    Pro_Map.insert(std::make_pair(name,Pro));
    boost::thread(boost::bind(&Producer::run,Pro));
    pthread_mutex_unlock(&Pro_lock);
    return (0);
}

int
add_NDN_Producer(const string prefix,
                    const string name,
                    const void* data,
                    uint32_t size){
    Producer *Pro = new Producer(prefix,name,data,size);
    if (add_NDN_Producer_pool(Pro) < 0){
        free(Pro);
        return (-1);
    }
    return (0);
}

int
add_NDN_Producer(const string prefix,
                    const string name,
                    const string path){
    Producer *Pro = new Producer(prefix,name,path);
    if (add_NDN_Producer_pool(Pro) < 0){
        free(Pro);
        return (-1);
    }
    return (0);
}

int
add_NDN_Producer(const string prefix,
                    const string name,
                    const key_t key,
                    const uint8_t command,
                    const string Dname){
    Producer *Pro = new Producer(prefix,name,key,command,Dname);
    if (add_NDN_Producer_pool(Pro) < 0) {
        free(Pro);
        return (-1);
    }
    return (0);
}

int
del_NDN_Producer(const string prefix,
                    const string name){
    pthread_mutex_lock(&Pro_lock);
    Producer*   Pro;
    string      TempName=tools::createName(prefix,name);
    if (Pro_Map.find(TempName) == Pro_Map.end()) {
        log_err("del_NDN_Producer() can't find producer %s",TempName.c_str());
        pthread_mutex_lock(&Pro_lock);
        return (-1);
    }
    Pro = Pro_Map[TempName];
    Pro_Map.erase(TempName);
    Pro->finish();
    free(Pro);
    pthread_mutex_unlock(&Pro_lock);
    return 0;
}

}
