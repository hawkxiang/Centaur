#ifndef _TOOLS_COMMON_HPP_
#define _TOOLS_COMMON_HPP_

#include "../Common.hpp"

namespace tools {

int getLocalIP_1(char* outip);
int getLocalIP_2(char* outip);

string toString(int Num);
string createName(const string m_Name,const string m_Data);
string createInfoFileName(const string m_Data,const string m_Attr);

}//namespace tools

/*
 * log information
 * output to terminal or file.
 */
void set_ver_debug(int num);
void set_App_out(const char* Path);
void set_file_out(const char* Path);
void close_file_out();
void log_debug(const char*, ...);
void log_time(const char*, ...);
void log_info(const char*, ...);
void log_err(const char*, ...);
void log_App(const char*, ...);

#endif//_TOOLS_COMMON_HPP_
