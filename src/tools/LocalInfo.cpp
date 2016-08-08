#include "Common.hpp"

/* Get Local IP for Linux Socket
 * Mac OS didn't use the method
 * @param {char*} outip
 * @return {int}
 * @return 0 find the Local IP address
 * @return -1 can't find the Local IP address
 */
int tools::getLocalIP_1(char* outip)
{
    int sockfd;
    char* ip;
    char buf[512];
    struct ifreq *ifreq;

    struct ifconf ifconf;
    ifconf.ifc_len = 512;
    ifconf.ifc_buf = buf;

    if ((sockfd = socket(AF_INET, SOCK_DGRAM,0)) < 0)
    {
        return -1;
    }
    ioctl(sockfd,SIOCGIFCONF,&ifconf);
    close(sockfd);

    ifreq = (struct ifreq*)buf;

    for (int i = (ifconf.ifc_len/sizeof(struct ifreq)); i > 0; i--)
    {
        ip = inet_ntoa(((struct sockaddr_in*)&(ifreq->ifr_addr))->sin_addr);
        if(strcmp(ip,"127.0.0.1")==0)
        {
            ifreq++;
            continue;
        }
        strcpy(outip,ip);
        return 0;
    }
    return -1;
}

/* get Local IP in Boost library ASIO
 * @param {char*}outIP
 * @return {int}
 * @return 0 find the Local IP address
 * @return -1 can't find the Local IP address
 */
int tools::getLocalIP_2(char* outip)
{
    boost::asio::io_service IP_ios;

    boost::asio::ip::tcp::resolver resolver(IP_ios);
    boost::asio::ip::tcp::resolver::query query(boost::asio::ip::host_name(),"");
    boost::asio::ip::tcp::resolver::iterator iter = resolver.resolve(query);
    boost::asio::ip::tcp::resolver::iterator end;

    while (iter != end)
    {
        boost::asio::ip::tcp::endpoint endpoint = *iter++;
        if ((endpoint.address().is_v4()) and
            (endpoint.address().to_string() != "127.0.0.1") and
            (endpoint.address().to_string() != "127.0.1.1"))
        {
            strcpy(outip,endpoint.address().to_string().c_str());
            return 0;
        }
    }
    return -1;
}
