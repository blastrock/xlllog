#include <array>
#include <map>
#include <cassert>
#include <cstring>

#include <xll/pnt.hpp>

namespace xll
{
namespace log
{

namespace detail
{

struct Settings
{
  Handler* handler = nullptr;
  int level = LEVEL_SILENT;
  std::map<const char*, int> catlevels;
};

inline
Settings& getSettingsStorage()
{
  static Settings settings;
  return settings;
}

inline
Handler* getHandlerPtr()
{
  assert(getSettingsStorage().handler);
  return getSettingsStorage().handler;
}

template <typename CharT, typename Traits = std::char_traits<CharT>>
class StreamBuf
{
public:
  using char_type = CharT;
  using traits_type = Traits;

  ~StreamBuf()
  {
    flush();
  }

  void sputc(char c)
  {
    _buf[_pos] = c;
    ++_pos;

    flushIfNeeded();
  }

  void sputn(const char* s, std::size_t n)
  {
    while (n)
    {
      auto size = std::min(_buf.size() - _pos, n);
      std::memcpy(&_buf[_pos], s, size);
      _pos += size;
      s += size;
      n -= size;

      flushIfNeeded();
    }
  }

private:
  std::array<char, 64> _buf;
  uint8_t _pos = 0;

  void flushIfNeeded()
  {
    assert(_pos <= _buf.size());

    if (_pos == _buf.size())
      flush();
  }

  void flush()
  {
    if (!_pos)
      return;

    detail::getHandlerPtr()->feed(&_buf[0], _pos);

    _pos = 0;
  }
};

class ScopedLog
{
public:
  ScopedLog(
      int level,
      const char* category,
      const char* file,
      unsigned int line)
  {
    detail::getHandlerPtr()->beginLog(level, category, file, line);
  }

  ~ScopedLog()
  {
    detail::getHandlerPtr()->endLog();
  }
};

}

inline
void setHandler(Handler* hndl)
{
  detail::getSettingsStorage().handler = hndl;
}

inline
void setLevel(int level)
{
  detail::getSettingsStorage().level = level;
}

inline
void addFilter(const char* category, int level)
{
  detail::getSettingsStorage().catlevels[category] = level;
}

template <typename... Args>
void log(
    int level,
    const char* category,
    const char* file,
    unsigned int line,
    const char* format,
    Args... args)
{
  const auto& catlevels = detail::getSettingsStorage().catlevels;
  const auto iter = catlevels.find(category);
  if (iter != catlevels.end())
  {
    if (level > iter->second)
      return;
  }
  else if (level > detail::getSettingsStorage().level)
    return;

  detail::ScopedLog _(level, category, file, line);

  detail::StreamBuf<char> sb;
  pnt::writef(sb, format, args...);
}

}
}
