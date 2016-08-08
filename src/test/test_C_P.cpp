#include "../Common.hpp"
#include "../NDN_Module/Consumer.hpp"
#include "../NDN_Module/Producer.hpp"
#include "../tools/Common.hpp"

int main(){

    log_time("Begin");

    init_Storage_Module();

    File_Data FS;
    boost::filesystem::path in("/tmp/in.pdf");
    boost::filesystem::path out("/tmp/out.pdf");
    FS.Size = boost::filesystem::file_size(in);
    FS.Seg = ((FS.Size - 1) / DATA_8K) + 1;
    log_debug("File Size: %d File Seg: %d",FS.Size,FS.Seg);

    ndn::add_NDN_Producer("Test","Data1",&FS,sizeof(FS));
    ndn::add_NDN_Producer("Test","Data2",in.c_str());

    log_debug("build Producer");

    ndn::Consumer<File_Data> *C1 =
        new ndn::Consumer<File_Data>("Test","Data1");
    C1->opt_Retry_Data(0,1);
    File_Data TempFS;
    C1->getData(&TempFS);
    log_debug("File Size: %d",TempFS.Size);
    log_debug("File Seg: %d",TempFS.Seg);

    ndn::Consumer<Data_8K> *C2 =
        new ndn::Consumer<Data_8K>("Test","Data2",out.c_str(),TempFS.Seg);
    C2->opt_Retry_Data(0,TempFS.Seg);

    log_debug("-----clear-----");
    C1->finish();
    free(C1);
    C2->finish();
    free(C2);
    log_debug("free Consumer");
    ndn::del_NDN_Producer("Test","Data1");
    log_debug("free Producer 1");
    ndn::del_NDN_Producer("Test","Data2");
    log_debug("free Producer 2");
    log_debug("-----clear end-----");

    log_time("End");
    return 0;
}
