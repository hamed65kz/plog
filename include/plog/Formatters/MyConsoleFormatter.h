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

        static util::nstring format(const Record& record)
        {
            tm t;
            util::localtime_s(&t, &record.getTime().time);

            util::nostringstream ss;
            //ss << t.tm_year + 1900 << "-" << std::setfill(PLOG_NSTR('0')) << std::setw(2) << t.tm_mon + 1 << PLOG_NSTR("-") << std::setfill(PLOG_NSTR('0')) << std::setw(2) << t.tm_mday << PLOG_NSTR(" ");
            ss << std::setfill(PLOG_NSTR('0')) << std::setw(2) << t.tm_hour << PLOG_NSTR(":") << std::setfill(PLOG_NSTR('0')) << std::setw(2) << t.tm_min << PLOG_NSTR(":") << std::setfill(PLOG_NSTR('0')) << std::setw(2) << t.tm_sec << PLOG_NSTR(".") << std::setfill(PLOG_NSTR('0')) << std::setw(3) << static_cast<int> (record.getTime().millitm) << PLOG_NSTR(" ");
            ss << std::setfill(PLOG_NSTR(' '))<< PLOG_NSTR("[")<<PLOG_NSTR(START_TO_COLOR_DELIMTER) << std::setw(5) << std::left << severityToString(record.getSeverity()) <<PLOG_NSTR(END_OF_COLOR_DELIMTER)<< PLOG_NSTR("]");
            //ss << PLOG_NSTR("[") << record.getTid() << PLOG_NSTR("] ");
            ss << PLOG_NSTR("[")<< std::setw(37)<< std::left << record.getFunc() << PLOG_NSTR("@")<< std::setw(3) <<record.getLine() << PLOG_NSTR("] ");
            ss <<PLOG_NSTR(START_TO_COLOR_DELIMTER)<< record.getMessage() <<PLOG_NSTR(END_OF_COLOR_DELIMTER)<< PLOG_NSTR("\n");

            return ss.str();
        }
    };
}

