#ifndef _COMMON_HPP_
#define _COMMON_HPP_

//Stand library
#include <iostream>
#include <stdio.h>
#include <cstdio>
#include <string.h>
#include <vector>
#include <unistd.h>
#include <string>
#include <net/if.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/msg.h>
#include <errno.h>

//ndn-cxx library
#include <ndn-cxx/data.hpp>
#include <ndn-cxx/face.hpp>
#include <ndn-cxx/interest.hpp>
#include <ndn-cxx/security/key-chain.hpp>
#include <ndn-cxx/util/scheduler.hpp>
#include <ndn-cxx/util/signal.hpp>

//Boost library
#include <boost/asio.hpp>
#include <boost/noncopyable.hpp>
#include <boost/smart_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/assert.hpp>
#include <boost/thread.hpp>
#include <boost/filesystem.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "Constant.hpp"

#endif//_COMMON_HPP_
