#include "../Common.hpp"
#include "../IP_Module/Client.hpp"
#include "../IP_Module/Server.hpp"
#include "../tools/Common.hpp"

int main(){

    log_time("Begin");

    init_Storage_Module();

    struct IP_v4_Address IP;
    tools::getLocalIP_2(IP.Address);
    log_debug("%s %d",IP.Address,IP.Port);
    ip::init_Server_pool(IP.Port);

    ip::run_TCP_Server(20);

    File_Data FS;
    boost::filesystem::path in("/tmp/testdata");
    boost::filesystem::path out("/tmp/testdata2");
    FS.Size = boost::filesystem::file_size(in);
    FS.Seg = ((FS.Size - 1) / DATA_1K) + 1;
    log_debug("File Size: %d File Seg: %d",FS.Size,FS.Seg);

    ip::add_TCP_Server("Data1",&FS,sizeof(FS));
    ip::add_TCP_Server("Data2",in.c_str());

    ip::Client<File_Data> *C1 = new ip::Client<File_Data>(IP,"Data1");

    C1->run_TCP();
    C1->opt_TCP(0,1);
    File_Data TempFS;
    C1->getData(&TempFS);
    log_debug("File Size: %d",TempFS.Size);
    log_debug("File Seg %d",TempFS.Seg);

    ip::Client<Data_8K> *C2 =
        new ip::Client<Data_8K>(IP,"Data2",out.c_str(),TempFS.Seg);
    C2->run_TCP();
    C2->opt_TCP(0,TempFS.Seg);

    log_debug("Test complete");

    log_debug("-----clear-----");
    C1->finish_TCP();
    free(C1);
    log_debug("free C1");
    C2->finish_TCP();
    free(C2);
    log_debug("free C2");
    ip::del_TCP_Server("Data1");
    ip::del_TCP_Server("Data2");
    ip::close_TCP_Server();
    log_debug("-----clear end-----");

    log_time("End");
    return 0;
}
