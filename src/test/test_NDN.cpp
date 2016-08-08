#include "../Common.hpp"
#include "../NDN_Module/Producer.hpp"
#include "../NDN_Module/Consumer.hpp"
#include "../tools/Common.hpp"

int main(){
    log_time("Begin");

    init_Storage_Module();

    boost::filesystem::path out("/tmp/test");
    ndn::add_NDN_Producer("Test","D1",out.c_str());

    log_time("Start Test1...");
    ndn::Consumer<Data_8K> *T8 = new ndn::Consumer<Data_8K>("Test","D1");
    ndn::Consumer<Data_4K> *T4 = new ndn::Consumer<Data_4K>("Test","D1");
    ndn::Consumer<Data_2K> *T2 = new ndn::Consumer<Data_2K>("Test","D1");
    ndn::Consumer<Data_1K> *T1 = new ndn::Consumer<Data_1K>("Test","D1");

    for(int i=0;i<2;i++){
        log_time("Loop %d",i);
        log_time("[Start] 8K with Signature");
        T8->opt_Retry_Data(0,1);
        log_time("[End  ] 8K with Signature");
        log_time("[Start] 4K with Signature");
        T4->opt_Retry_Data(0,1);
        log_time("[End  ] 4K with Signature");
        log_time("[Start] 2K with Signature");
        T2->opt_Retry_Data(0,1);
        log_time("[End  ] 2K with Signature");
        log_time("[Start] 1K with Signature");
        T1->opt_Retry_Data(0,1);
        log_time("[End  ] 1K with Signature");
    }


    ndn::del_NDN_Producer("Test","D1");
    T8->finish();
    T4->finish();
    T2->finish();
    T1->finish();
    free(T8);
    free(T4);
    free(T2);
    free(T1);
    log_time("End");
    return 0;
}
