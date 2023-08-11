#pragma once
#include <plog/Appenders/ConsoleAppender.h>
#include <plog/WinApi.h>
#include <iostream>
#include <vector>

#define FOREGROUND_BLUE      0x0001 // text color contains blue.
#define FOREGROUND_GREEN     0x0002 // text color contains green.
#define FOREGROUND_RED       0x0004 // text color contains red.
#define FOREGROUND_INTENSITY 0x0008 // text color is intensified.
#define BACKGROUND_BLUE      0x0010 // background color contains blue.
#define BACKGROUND_GREEN     0x0020 // background color contains green.
#define BACKGROUND_RED       0x0040 // background color contains red.
#define BACKGROUND_INTENSITY 0x0080 // background color is intensified.


#define RGB_WhiteOnBlack   "\x1B[38;2;255;255;255m"
#define RGB_RedOnBlack     "\x1B[38;2;255;102;102m"
#define RGB_BlueOnBlack    "\x1B[38;2;0;128;255m"
#define RGB_GrayOnBlack    "\x1B[38;2;120;120;120"
#define RGB_YellowOnBlack  "\x1B[38;2;255;225;0m"
#define RGB_BlackOnRed  "\x1b[41;30m"

#define START_TO_COLOR_DELIMTER  '^'
#define END_OF_COLOR_DELIMTER  '&'

enum WinConsoleColors
{
   BlackFore   = 0,
   MaroonFore  = FOREGROUND_RED,
   GreenFore   = FOREGROUND_GREEN,
   NavyFore    = FOREGROUND_BLUE,
   TealFore    = FOREGROUND_GREEN | FOREGROUND_BLUE,
   OliveFore   = FOREGROUND_RED | FOREGROUND_GREEN,
   PurpleFore  = FOREGROUND_RED | FOREGROUND_BLUE,
   GrayFore    = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,
   SilverFore  = FOREGROUND_INTENSITY,
   RedFore     = FOREGROUND_INTENSITY | FOREGROUND_RED,
   LimeFore    = FOREGROUND_INTENSITY | FOREGROUND_GREEN,
   BlueFore    = FOREGROUND_INTENSITY | FOREGROUND_BLUE,
   AquaFore    = FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_BLUE,
   YellowFore  = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN,
   FuchsiaFore = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_BLUE,
   WhiteFore   = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,

   BlackBack   = 0,
   MaroonBack  = BACKGROUND_RED,
   GreenBack   = BACKGROUND_GREEN,
   NavyBack    = BACKGROUND_BLUE,
   TealBack    = BACKGROUND_GREEN | BACKGROUND_BLUE,
   OliveBack   = BACKGROUND_RED | BACKGROUND_GREEN,
   PurpleBack  = BACKGROUND_RED | BACKGROUND_BLUE,
   GrayBack    = BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE,
   SilverBack  = BACKGROUND_INTENSITY,
   RedBack     = BACKGROUND_INTENSITY | BACKGROUND_RED,
   LimeBack    = BACKGROUND_INTENSITY | BACKGROUND_GREEN,
   BlueBack    = BACKGROUND_INTENSITY | BACKGROUND_BLUE,
   AquaBack    = BACKGROUND_INTENSITY | BACKGROUND_GREEN | BACKGROUND_BLUE,
   YellowBack  = BACKGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_GREEN,
   FuchsiaBack = BACKGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_BLUE,
   WhiteBack   = BACKGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE,
};

namespace plog
{

    template<class Formatter>
    class PLOG_LINKAGE_HIDDEN ColorConsoleAppender : public ConsoleAppender<Formatter>
    {
    public:
#ifdef _WIN32
#   ifdef _MSC_VER
#       pragma warning(suppress: 26812) //  Prefer 'enum class' over 'enum'
#   endif
        ColorConsoleAppender(OutputStream outStream = streamStdOut)
            : ConsoleAppender<Formatter>(outStream)
            , m_originalAttr()
        {
            if (this->m_isatty)
            {
                CONSOLE_SCREEN_BUFFER_INFO csbiInfo;
                GetConsoleScreenBufferInfo(this->m_outputHandle, &csbiInfo);

                m_originalAttr = csbiInfo.wAttributes;
            }
        }
#else
        ColorConsoleAppender(OutputStream outStream = streamStdOut)
            : ConsoleAppender<Formatter>(outStream)
        {}
#endif
        std::vector<std::wstring> splitBasedColor(util::nstring str)
        {         
            std::vector<std::wstring> segments;
            auto ind1 = str.find_first_of(START_TO_COLOR_DELIMTER,0);
            auto ind2 = str.find_first_of(END_OF_COLOR_DELIMTER,0);
            while(ind1 >0 && ind2>0 && ind1< str.size()){

                segments.push_back(str.substr(0,ind1));
                segments.push_back(str.substr(ind1,ind2-ind1));
                str.erase(0,ind2+1);
                ind1 = str.find_first_of(START_TO_COLOR_DELIMTER,0);
                ind2 = str.find_first_of(END_OF_COLOR_DELIMTER,0);
            }
            segments.push_back(str);
            return segments;
        }
        virtual void write(const Record& record) PLOG_OVERRIDE
        {
            util::nstring str = Formatter::format(record);
            util::MutexLock lock(this->m_mutex);
            std::vector<std::wstring> segs = splitBasedColor(str);
            for (int i=0;i<segs.size();i++) {
                std::wstring s=segs[i];
                if(s[0] == START_TO_COLOR_DELIMTER){
                    s = s.erase(0,1);
                    setColor(record.getSeverity());
                }
                else{
                     setColor(Severity::verbose);
                }
                this->writestr(s);
                resetColor();
            }
            //setColor(record.getSeverity());
            //this->writestr(str);
            //resetColor();
        }

    protected:
        void setColor(Severity severity)
        {
            if (this->m_isatty)
            {
                switch (severity)
                {
#ifdef _WIN32
                case fatal:
                    SetConsoleTextAttribute(this->m_outputHandle, static_cast<WORD>(WinConsoleColors::RedBack |  WinConsoleColors::BlackFore)); // white on red
                    break;

                case error:
                    SetConsoleTextAttribute(this->m_outputHandle, static_cast<WORD>(WinConsoleColors::BlackBack |  WinConsoleColors::RedFore)); // red
                    break;

                case warning:
                    SetConsoleTextAttribute(this->m_outputHandle, static_cast<WORD>(WinConsoleColors::BlackBack |  WinConsoleColors::YellowFore)); // yellow
                    break;

                case debug:
                    SetConsoleTextAttribute(this->m_outputHandle, static_cast<WORD>(WinConsoleColors::BlackBack |  WinConsoleColors::GrayFore)); // cyan
                    break;

                case verbose:
                    SetConsoleTextAttribute(this->m_outputHandle, static_cast<WORD>(WinConsoleColors::BlackBack |  WinConsoleColors::SilverFore)); // cyan
                    break;

                case info:
                    SetConsoleTextAttribute(this->m_outputHandle, static_cast<WORD>(WinConsoleColors::BlackBack |  WinConsoleColors::BlueFore)); // cyan
                    break;
#else
                case fatal:
                    this->m_outputStream << RGB_BlackOnRed ; // white on red background
                    break;

                case error:
                    this->m_outputStream << RGB_RedOnBlack; // red
                    break;

                case warning:
                    this->m_outputStream << RGB_YellowOnBlack; // yellow
                    break;

                case debug:
                    this->m_outputStream << RGB_WhiteOnBlack; // cyan
                    break;

                case verbose:
                    this->m_outputStream << RGB_GrayOnBlack; // cyan
                    break;

                case info:
                    this->m_outputStream << RGB_BlueOnBlack; // cyan
                    break;
#endif
                default:
                    break;
                }
            }
        }

        void resetColor()
        {
            if (this->m_isatty)
            {
#ifdef _WIN32
                SetConsoleTextAttribute(this->m_outputHandle, m_originalAttr);
#else
                this->m_outputStream << "\x1B[0m\x1B[0K";
#endif
            }
        }

    private:
#ifdef _WIN32
        WORD   m_originalAttr;
#endif
    };
}
