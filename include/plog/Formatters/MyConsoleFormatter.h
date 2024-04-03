#pragma once
#include <plog/Record.h>
#include <plog/Util.h>
#include <iomanip>
namespace plog
{
    #define START_TO_COLOR_DELIMTER '^'
    #define END_OF_COLOR_DELIMTER '&'
    class MyConsoleFormatter
    {
    public:
        static util::nstring header()
        {
            return util::nstring();
        }
       static  std::string& truncate(std::string& str, size_t width, bool show_ellipsis=true) {
            if (str.length() > width) {
                if (show_ellipsis) {
                    str.resize(width);
                    return str.append("...");
                }
                else {
                    str.resize(width);
                    return str;
               }
            }
            return str;
        }
       static  std::wstring& truncate(std::wstring& str, size_t width, bool show_ellipsis=true) {
            if (str.length() > width) {
                if (show_ellipsis) {
                    str.resize(width);
                    return str.append(L"...");
                }
                else {
                    str.resize(width);
                    return str;
               }
            }
            return str;
        }
       static util::nstring format(const Record& record)
        {
            tm t;
            util::localtime_s(&t, &record.getTime().time);

            std::string funcstr = record.getFunc();
            funcstr = truncate(funcstr,37,true);

#if PLOG_CHAR_IS_UTF8
            char msg[300]={0};
            snprintf(msg,300,"%s",record.getMessage());
            std::string msgstr = std::string(msg);
            msgstr = truncate(msgstr,110,true);
#else
            std::wstring ws(record.getMessage());
            std::wstring msgstr = truncate(ws,110,true);
#endif
            util::nostringstream ss;
            //ss << t.tm_year + 1900 << "-" << std::setfill(PLOG_NSTR('0')) << std::setw(2) << t.tm_mon + 1 << PLOG_NSTR("-") << std::setfill(PLOG_NSTR('0')) << std::setw(2) << t.tm_mday << PLOG_NSTR(" ");
            ss << std::setfill(PLOG_NSTR('0')) << std::setw(2) << t.tm_hour << PLOG_NSTR(":") << std::setfill(PLOG_NSTR('0')) << std::setw(2) << t.tm_min << PLOG_NSTR(":") << std::setfill(PLOG_NSTR('0')) << std::setw(2) << t.tm_sec << PLOG_NSTR(".") << std::setfill(PLOG_NSTR('0')) << std::setw(3) << static_cast<int> (record.getTime().millitm) << PLOG_NSTR(" ");
            ss << std::setfill(PLOG_NSTR(' '))<< PLOG_NSTR("[")<<PLOG_NSTR(START_TO_COLOR_DELIMTER) << std::setw(5) << std::left << severityToString(record.getSeverity()) <<PLOG_NSTR(END_OF_COLOR_DELIMTER)<< PLOG_NSTR("]");
            //ss << PLOG_NSTR("[") << record.getTid() << PLOG_NSTR("] ");
            ss << PLOG_NSTR("[")<< std::setw(37)<< std::left << funcstr.c_str() << PLOG_NSTR("@")<< std::setw(3) <<record.getLine() << PLOG_NSTR("] ");
            ss <<PLOG_NSTR(START_TO_COLOR_DELIMTER)<< msgstr.c_str() <<PLOG_NSTR(END_OF_COLOR_DELIMTER)<< PLOG_NSTR("\n");

            return ss.str();
        }
    };
}

