#include "../tools/Common.hpp"

void getSystemInfo()
{
    switch (__SYSTEM__)
    {
        case 2:
            log_info("System MacOS");
            break;
        case 1:
            log_info("System Unix/Linux");
            break;
        default:
            log_info("Other System");
            break;
    }
}

void check_Data_size()
{
    printf("IP_V4 Address Size: %lu\n",sizeof(IP_v4_Address));
    printf("Data 1KB Size: %lu\n",sizeof(Data_1K));
    printf("Data 2KB Size: %lu\n",sizeof(Data_2K));
    printf("Data 4KB Size: %lu\n",sizeof(Data_4K));
    printf("Data 8KB Size: %lu\n",sizeof(Data_8K));
}

int main()
{
    printf("=====System Info=====\n");
    getSystemInfo();
    printf("=====IP Address=====\n");
    char ip[20];
    if (tools::getLocalIP_1(ip) == 0)
    {
        printf("Linux Socket get Local IP is: %s\n",ip);
    }
    else
    {
        printf("Linux Socket can't find Local IP\n");
    }

    char ip2[20];
    if (tools::getLocalIP_2(ip2) == 0)
    {
        printf("Boost asio get Local IP is: %s\n",ip2);
    }
    else
    {
        printf("Boost asio can't find Local IP\n");
    }
    printf("=====Data size=====\n");
    check_Data_size();
    return 0;
}
