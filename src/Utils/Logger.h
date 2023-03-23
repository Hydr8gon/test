#ifndef _LOGGER_H

#ifndef NO_SDL
#include <spdlog/spdlog.h>
#endif

#include <string>

namespace NXE
{
namespace Utils
{

namespace Logger
{

  void init(std::string filename);

#ifdef NO_SDL
#define LOG_TRACE(...)    { printf(__VA_ARGS__); printf("\n"); }
#define LOG_DEBUG(...)    { printf(__VA_ARGS__); printf("\n"); }
#define LOG_INFO(...)     { printf(__VA_ARGS__); printf("\n"); }
#define LOG_WARN(...)     { printf(__VA_ARGS__); printf("\n"); }
#define LOG_ERROR(...)    { printf(__VA_ARGS__); printf("\n"); }
#define LOG_CRITICAL(...) { printf(__VA_ARGS__); printf("\n"); }
#else
#define LOG_TRACE SPDLOG_TRACE
#define LOG_DEBUG SPDLOG_DEBUG
#define LOG_INFO SPDLOG_INFO
#define LOG_WARN SPDLOG_WARN
#define LOG_ERROR SPDLOG_ERROR
#define LOG_CRITICAL SPDLOG_CRITICAL
#endif

} // namespace Logger
} // namespace Utils
} // namespace NXE

#endif //_LOGGER_H
