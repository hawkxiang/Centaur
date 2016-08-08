#include "Common.hpp"
/*
 * integer to string.
 * @param   {int} Num
 * @return  {string}
 */
string
tools::toString(int Num) {
    std::ostringstream outS;
    outS << Num;
    return string(outS.str());
}

/*
 * crate name with NDN Prefix.
 * @param   {string} m_Name
 * @param   {string} m_Data
 * @return  {string}
 */
string
tools::createName(const string m_Name,
                    const string m_Data) {
    string TempStr("/");
    TempStr += (m_Name+"/");
    TempStr += (m_Data+"/");
    return (TempStr);
}

/*
 * create Info_File name.
 * @param   {string} m_Data
 * @param   {string} m_Attr
 * @return  {string}
 * Default info file dir see INFOFILEDIR
 */
string
tools::createInfoFileName(const string m_Data,
                            const string m_Attr) {
    string TempStr(INFOFILEDIR);
    TempStr += "/";
    TempStr += (m_Data+"_");
    TempStr += tools::toString(getpid());
    TempStr += (m_Attr);
    return (TempStr);
}
