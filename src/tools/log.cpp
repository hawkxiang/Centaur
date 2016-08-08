#include "Common.hpp"

#include <stdarg.h>

#define MAXLINE 4096
//default ver debug infomation open
static int ver_debug = 1;
static FILE* logOut = stdout;
static FILE* AppLog = stdout;

static void log_doit(int level,const char *,va_list);

pthread_mutex_t     log_lock = PTHREAD_MUTEX_INITIALIZER;

void
set_ver_debug(int num){
    ver_debug = num;
}

void set_App_out(const char* Path){
    boost::filesystem::path out(Path);
    AppLog = fopen(out.c_str(),"w+");
    if (AppLog == NULL) {
        AppLog = stdout;
    }
}
void set_file_out(const char* Path){
    boost::filesystem::path out(Path);
    logOut = fopen(out.c_str(),"w+");
    if (logOut == NULL) {
        logOut = stdout;
    }
}
void close_file_out() {
    if (logOut != stdout){
        fclose(logOut);
        logOut = stdout;
    }
}

/*
 * output debug msg.
 */
void
log_debug(const char *fmt, ...) {
    va_list     ap;
    if (ver_debug == 0)
        return;
    va_start(ap,fmt);
    log_doit(0,fmt,ap);
    va_end(ap);
}

/*
 * output msg with time.
 */
void
log_time(const char *fmt, ...) {
    va_list         ap;
    va_start(ap,fmt);
    log_doit(1,fmt,ap);
    va_end(ap);
}

/*
 * output info msg.
 */
void
log_info(const char *fmt, ...) {
    va_list     ap;
    va_start(ap,fmt);
    log_doit(2,fmt,ap);
    va_end(ap);
}

/*
 * output error msg.
 */
void
log_err(const char *fmt, ...) {
    va_list     ap;
    va_start(ap,fmt);
    log_doit(3,fmt,ap);
    va_end(ap);
}

/*
 * output App log info.
 */
void
log_App(const char *fmt, ...){
    va_list     ap;
    va_start(ap,fmt);
    log_doit(4,fmt,ap);
    va_end(ap);
}

/*
 * output msg.
 * pthread mutex: log_lock;
 */
static void
log_doit(int level,const char *fmt,va_list ap) {
    pthread_mutex_lock(&log_lock);

    char    buf[MAXLINE];
    char    buf2[MAXLINE];

    switch (level) {
        case 0:
            snprintf(buf2,sizeof(buf2),"%18s: ","debug msg");
            break;
        case 1:
            struct timeval now;
            gettimeofday(&now,NULL);
            snprintf(buf2,sizeof(buf2),"%011ld.%06ld: ",now.tv_sec,(long)now.tv_usec);
            break;
        case 2:
            snprintf(buf2,sizeof(buf2),"%18s: ","info msg");
            break;
        case 3:
            snprintf(buf2,sizeof(buf2),"%18s: ","error msg");
            break;
        case 4:
            snprintf(buf2,sizeof(buf2),"%18s: ","App Log msg");
            break;
    }

    vsnprintf(buf,sizeof(buf),fmt,ap);
    strcat(buf,"\n");
    if (level == 4){
        fputs(buf2,AppLog);
        fputs(buf,AppLog);
        fflush(AppLog);
    } else {
        fputs(buf2,logOut);
        fputs(buf,logOut);
        fflush(logOut);
        if ((logOut != stdout) && (level != 0)) {
            fputs(buf2,stdout);
            fputs(buf,stdout);
            fflush(stdout);
        }
    }
    pthread_mutex_unlock(&log_lock);
}
