# NDN-Centaur

##Complete Module
* Storage Module
* IP Module
    * Client
    * Server
* NDN Module
    * Consumer
    * Producer
* Tools Module
    * get Local IP address(Boost lib and Linux socket)
    * log output
* Centaur Application
    * System V IPC
    * Add Del Resource Data
    * Proxy Node
    * Prefetch Data
    * Basic Req
    * Pipeline Req
    * NDN Req

##
* Using Boost thread lib
* Using Linux system socket
* Using Linux system V message queue

##Build Tools
* [Waf](https://waf.io/)

##Other tools
* calc.sh: calculate src/ total line.

##App Command
* Centaur -p Prot -q backlog -f fileout -l AppLog -d
* C\_Quit
* C\_PROXY DataName
* C\_PREFETCH DataName Percent
* C\_ADD DataName inPath IPwaitTime
* C\_DEL DataName
* C\_REQ\_B DataName outPath
* C\_REQ\_P DataName outPath AppLog
* C\_REQ\_N DataName outPath
