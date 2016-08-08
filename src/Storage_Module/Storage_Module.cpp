#include "Storage_Module.hpp"

std::map<string,int>                nametofd;
std::map<int,string>                fdtoname;
std::map<string,string>             nametopath;
std::map<int,struct Storage_FD *>   fdcount;

pthread_mutex_t                     Storage_lock = PTHREAD_MUTEX_INITIALIZER;


static void     S_M_close_file(const string name,int fd);
static int      init_DIR();

/*
 * Initialize Storage Module.
 * Build Storage info directory.
 * Call init_DIR()
 */
int
init_Storage_Module() {
    log_info("Init Storage Module...");
    if (init_DIR() < 0) {
        log_err("Init_Storage_Module() can't build DIR.");
        return (-1);
    }
    return (0);
}

/*
 * open file.
 * @param : {string}name
 * @param : {string}path
 * @param : {int}oflag
 * @return >= 0 : open file successful.
 * @return <  0 : open file failed.
 * LOCKING : Storage_lock.
 */
int
open_file(const string name,const string path,int oflag) {
    pthread_mutex_lock(&Storage_lock);
    int                 fd;
    auto                map_it = nametofd.find(name);
    struct Storage_FD   *Temp;
    if (map_it != nametofd.end()) {
        if (nametopath.find(name)->second != path) {
            log_err("open_file() name to different file %s.",path.c_str());
            fd = -1;
            goto open_file_out;
        }
        fd = map_it->second;
        fdcount[fd]->fd_count++;
        goto open_file_out;
    }
    if ((fd = open(path.c_str(),oflag)) < 0) {
        log_err("open_file() can't open file %s.",path.c_str());
        fd = -1;
        goto open_file_out;
    }
    nametofd.insert(std::make_pair(name,fd));
    fdtoname.insert(std::make_pair(fd,name));
    nametopath.insert(std::make_pair(name,path));
    Temp = (struct Storage_FD*)malloc(sizeof(struct Storage_FD));
    Temp->fd_count = 1;
    pthread_mutex_init(&(Temp->fd_lock),NULL);
    fdcount.insert(std::make_pair(fd,Temp));

open_file_out:
    pthread_mutex_unlock(&Storage_lock);
    return (fd);
}

/*
 * create file.
 * @param : {string} name
 * @param : {string} path
 * @return >= 0 : create/open file successful and return fd.
 * @return < 0  : create/open file failed.
 * LOCKING : Storage_lock.
 */
int
create_file(const string name,const string path) {
    pthread_mutex_lock(&Storage_lock);
    int     fd;
    auto    map_it = nametofd.find(name);
    if (map_it != nametofd.end()) {
        fd = map_it->second;
        fdcount[fd]->fd_count++;
        goto create_file_out;
    }
    // create file.
    log_debug("create Path : %s.",path.c_str());
    if ((fd = creat(path.c_str(),FILE_MODE)) < 0) {
        log_err("create_file() : can't create file.");
        fd = -1;
        goto create_file_out;
    }
    close(fd);
    pthread_mutex_unlock(&Storage_lock);
    return (open_file(name,path,RWFILE));

create_file_out:
    pthread_mutex_unlock(&Storage_lock);
    return (fd);
}

/*
 * close file with name
 * @param : {string} name
 * @return <  0 : close error
 * @return >= 0 : remaining open count
 * LOCKING : Storage_lock
 */
int
close_file(const string name) {
    pthread_mutex_lock(&Storage_lock);
    int     fd;
    int     ret;
    auto    map_it = nametofd.find(name);
    if (map_it == nametofd.end()) {
        log_err("close_file() can't close file %s.",name.c_str());
        ret = -1;
        goto close_file_name_out;
    }
    fd = map_it->second;
    fdcount[fd]->fd_count--;
    ret = fdcount[fd]->fd_count;
    if (ret == 0) {
        S_M_close_file(name,fd);
    }

close_file_name_out:
    pthread_mutex_unlock(&Storage_lock);
    return (ret);
}

/*
 * close file with fd
 * @param : {int} fd
 * @return <  0 : close error
 * @return >= 0 : remaining open count
 * LOCKING : Storage_lock
 */
int
close_file(int fd) {
    pthread_mutex_lock(&Storage_lock);
    string  name;
    int     ret;
    auto    map_it = fdtoname.find(fd);
    if (map_it == fdtoname.end()) {
        log_err("close_file(): can't close file %d.",fd);
        ret = -1;
        goto close_file_fd_out;
    }
    name = map_it->second;
    fdcount[fd]->fd_count--;
    ret = fdcount[fd]->fd_count;
    if (ret == 0) {
        S_M_close_file(name,fd);
    }

close_file_fd_out:
    pthread_mutex_unlock(&Storage_lock);
    return (ret);
}

/*
 * delete file.
 * @param : {int} fd
 * @return 0  : close and delete file.
 * @return 1  : close file.
 * @return -1 : close error & file not exist.
 * Lock : Storage_lock
 */
int
delete_file(int fd) {
    pthread_mutex_lock(&Storage_lock);
    string  path;
    auto    map_it = fdtoname.find(fd);
    if (map_it == fdtoname.end()) {
        log_err("delete_file() file not exist %d.",fd);
        pthread_mutex_unlock(&Storage_lock);
        return (-1);
    }
    path = nametopath.find(map_it->second)->second;
    pthread_mutex_unlock(&Storage_lock);
    // close file and delete file.
    if (close_file(fd) == 0) {
        remove(path.c_str());
        log_debug("delete file success path %s.",path.c_str());
        return (0);
    }
    return (1);
}

/*
 * Check file open.
 * @param : {int} fd
 * @return <  0 : check error
 * @return >= 0 : file open count
 * Locking : Storage_lock
 */
int
check_file(int fd) {
    pthread_mutex_lock(&Storage_lock);
    int     ret;
    auto    map_it = fdcount.find(fd);
    if (map_it == fdcount.end()) {
        log_err("check_file(): file not exist %d.",fd);
        ret = -1;
        goto check_file_out;
    }
    ret = map_it->second->fd_count;

check_file_out:
    pthread_mutex_unlock(&Storage_lock);
    return (ret);
}

/*
 * free close file.
 * @param : {string} name
 * @param : {int} fd
 */
static void
S_M_close_file(const string name,int fd) {
    close(fd);
    nametofd.erase(name);
    fdtoname.erase(fd);
    nametopath.erase(name);
    auto Temp = fdcount[fd];
    pthread_mutex_destroy(&Temp->fd_lock);
    fdcount.erase(fd);
    free(Temp);
}

/*
 * Initialize and build directory
 * DATAFILEDIR & INFOFILEDIR
 */
static int
init_DIR() {
    if (!boost::filesystem::is_directory(DATAFILEDIR)) {
        log_debug("build DATA FILE DIR.");
        boost::filesystem::create_directory(DATAFILEDIR);
    }
    if (!boost::filesystem::is_directory(INFOFILEDIR)) {
        log_debug("build INFO FILE DIR.");
        boost::filesystem::create_directory(INFOFILEDIR);
    }
    return (0);
}

/*
 * Read Data from File(fd)
 * @param : {int} fd
 * @param : {uint32_t} Pos
 * @param : {uint8_t *} Data
 * @param : {uint32_t} Len
 * @return : read data size
 * Locking fd lock.
 */
uint32_t
readFileData(int fd,uint32_t Pos,uint8_t *Data,uint32_t Len) {
    uint32_t    ret;
    pthread_mutex_lock(&(fdcount[fd]->fd_lock));
    //lseek(fd,Pos,SEEK_SET);
    //ret = read(fd,Data,Len);
    ret = pread(fd,Data,Len,Pos);
    pthread_mutex_unlock(&(fdcount[fd]->fd_lock));
    return ret;
}

/*
 * Write Data to File(fd).
 * @param : {int} fd
 * @param : {uint32_t} Pos
 * @param : {const uint8_t *} Data
 * @param : {uint32_t} Len
 * @return : write data size
 * Locking fd lock.
 */
uint32_t
writeFileData(int fd,uint32_t Pos,const uint8_t *Data,uint32_t Len) {
    uint32_t    ret;
    pthread_mutex_lock(&(fdcount[fd]->fd_lock));
    //lseek(fd,Pos,SEEK_SET);
    //ret = write(fd,Data,Len);
    ret = pwrite(fd,Data,Len,Pos);
    pthread_mutex_unlock(&(fdcount[fd]->fd_lock));
    return ret;
}
