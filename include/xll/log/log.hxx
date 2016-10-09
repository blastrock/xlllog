// Copyright (c) 2013, Philippe Daouadi <p.daouadi@free.fr>
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met: 
// 
// 1. Redistributions of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimer. 
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution. 
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
// 
// The views and conclusions contained in the software and documentation are
// those of the authors and should not be interpreted as representing official
// policies, either expressed or implied, of the FreeBSD Project.

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
