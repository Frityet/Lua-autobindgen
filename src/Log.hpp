///
/// @author Amrit Bhogal on 03/06/2023
/// @brief 
/// @version 1.0.0
///
#pragma once

#include <llvm/Support/raw_ostream.h>

namespace LuaClang
{
    class Log {
    private:
        llvm::raw_fd_ostream &_stream;

    public:
        enum class Level {
            Debug,
            Info,
            Warning,
            Error,
            Fatal
        } level;

        std::string_view prefix;

        explicit Log(Level level) : _stream(level == Level::Debug || level == Level::Info ? llvm::outs() : llvm::errs()),
                                    level(level)
        {
            switch (level) {
                case Level::Debug:
                    prefix = "[LuaClang - Debug] ";
                    _stream.changeColor(llvm::raw_ostream::Colors::CYAN);
                    break;
                case Level::Info:
                    prefix = "[LuaClang - Info] ";
                    _stream.changeColor(llvm::raw_ostream::Colors::WHITE);
                    break;
                case Level::Warning:
                    prefix = "[LuaClang - Warning] ";
                    _stream.changeColor(llvm::raw_ostream::Colors::YELLOW);
                    break;
                case Level::Error:
                    prefix = "[LuaClang - Error] ";
                    _stream.changeColor(llvm::raw_ostream::Colors::RED);
                    break;
                case Level::Fatal:
                    prefix = "[LuaClang - Fatal] ";
                    _stream.changeColor(llvm::raw_ostream::Colors::RED);
                    break;
            }
        }

        template <typename ...T>
        void operator()(const T &...msg)
        {
            _stream << prefix;
            (_stream << ... << msg);
            _stream << '\n';

            if (level == Level::Fatal)
                exit(1);
        }

    }   debug   = Log(Log::Level::Debug),
        info    = Log(Log::Level::Info),
        warning = Log(Log::Level::Warning),
        error   = Log(Log::Level::Error),
        fatal   = Log(Log::Level::Fatal);
}
