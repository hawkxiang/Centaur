#include "../tools/Common.hpp"

int main(){
    set_ver_debug(1);
    log_time("begin");
    log_debug("hello world %d",1);
    sleep(1);
    set_file_out("out.txt");
    log_info("hello world %d",2);
    sleep(1);
    log_err("hello world %d",3);
    log_debug("hello world %d",4);
    close_file_out();
    log_time("end");
}

