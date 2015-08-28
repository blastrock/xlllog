#ifndef XLL_LOG_HPP
#define XLL_LOG_HPP

#include <streambuf>

#define XLL_LOG_CATEGORY(str) \
  static const char* XLL_LOG_CATEGORY_G = (str)

#define XLL_LOG(level, ...)                  \
  ::xll::log::log(::xll::log::LEVEL_##level, \
      XLL_LOG_CATEGORY_G,                    \
      __FILE__,                              \
      __LINE__,                              \
      __VA_ARGS__)

#define XLL_LOG_C(cat, level, ...)           \
  ::xll::log::log(::xll::log::LEVEL_##level, \
      cat,                                   \
      __FILE__,                              \
      __LINE__,                              \
      __VA_ARGS__)

#if defined(NDEBUG) || defined(XLL_LOG_NO_DEBUG)
#define xDeb(...)
#define xDebC(...)
#else
#define xDeb(...) XLL_LOG(DEBUG, __VA_ARGS__)
#define xDebC(cat, ...) XLL_LOG_C(cat, DEBUG, __VA_ARGS__)
#endif
#ifdef XLL_LOG_NO_VERBOSE
#define xVer(...)
#define xVerC(...)
#else
#define xVer(...) XLL_LOG(VERBOSE, __VA_ARGS__)
#define xVerC(cat, ...) XLL_LOG_C(cat, VERBOSE, __VA_ARGS__)
#endif
#ifdef XLL_LOG_NO_INFO
#define xInf(...)
#define xInfC(...)
#else
#define xInf(...) XLL_LOG(INFO, __VA_ARGS__)
#define xInfC(cat, ...) XLL_LOG_C(cat, INFO, __VA_ARGS__)
#endif
#ifdef XLL_LOG_NO_WARNING
#define xWar(...)
#define xWarC(...)
#else
#define xWar(...) XLL_LOG(WARNING, __VA_ARGS__)
#define xWarC(cat, ...) XLL_LOG_C(cat, WARNING, __VA_ARGS__)
#endif
#ifdef XLL_LOG_NO_ERROR
#define xErr(...)
#define xErrC(...)
#else
#define xErr(...) XLL_LOG(ERROR, __VA_ARGS__)
#define xErrC(cat, ...) XLL_LOG_C(cat, ERROR, __VA_ARGS__)
#endif
#ifdef XLL_LOG_NO_FATAL
#define xFat(...)
#define xFatC(...)
#else
#define xFat(...) XLL_LOG(FATAL, __VA_ARGS__)
#define xFatC(cat, ...) XLL_LOG_C(cat, FATAL, __VA_ARGS__)
#endif

namespace xll
{
namespace log
{

enum Level
{
  LEVEL_SILENT = 0,
  LEVEL_FATAL,
  LEVEL_ERROR,
  LEVEL_WARNING,
  LEVEL_INFO,
  LEVEL_VERBOSE,
  LEVEL_DEBUG,
};

class Handler
{
public:
  virtual void beginLog(
      int level,
      const char* category,
      const char* file,
      unsigned int line) = 0;
  virtual void feed(const char* s, std::size_t n) = 0;
  virtual void endLog() = 0;
};

void setHandler(Handler* handler);
void setLevel(int level);
void addFilter(const char* category, int level);

template <typename... Args>
void log(
    int level,
    const char* category,
    const char* file,
    unsigned int line,
    const char* format,
    Args... args);

}
}

#include "log/log.hxx"

#endif
