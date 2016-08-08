#include "../tools/Common.hpp"
#include "../Storage_Module/Storage_Module.hpp"

int main()
{
    boost::filesystem::path in("/tmp/testdata");
    boost::filesystem::path out("/tmp/testdata2");

    log_info("Read File: %s %s",boost::filesystem::system_complete(in).c_str(),in.c_str());
    log_info("File Size: %d",boost::filesystem::file_size(in));
    log_info("Write File: %s",boost::filesystem::system_complete(out).c_str());
    log_time("begin copy file");
    Data_8K TempData;
    log_debug("%d %d",sizeof(Data_8K),sizeof(TempData));
    init_Storage_Module();
    int fin = open_file("in",in.c_str(),RDFILE);
    int fout = create_file("out",out.c_str());
    if (fin < 0){
        log_err("error");
        return 0;
    }
    if (fout < 0){
        log_err("error");
        return 0;
    }
    size_t pos = 0;
    while (true)
    {
        size_t Len1 = readFileData(fin,pos,
                        TOCHARP(&TempData),sizeof(Data_8K));
        if (Len1 == 0)
            break;
        size_t Len2 = writeFileData(fout,pos,
                        TOCHARP(&TempData),Len1);
        pos += Len2;
        log_debug("%ld %ld %ld",pos,Len1,Len2);
    }
    log_time("end copy file");
    close_file(fin);
    delete_file(fout);
    return 0;
}
