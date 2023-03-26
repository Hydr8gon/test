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
#define LOG_TRACE(...)    (0)
#define LOG_DEBUG(...)    (0)
#define LOG_INFO(...)     (0)
#define LOG_WARN(...)     (0)
#define LOG_ERROR(...)    (0)
#define LOG_CRITICAL(...) (0)
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
