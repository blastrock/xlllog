#include <queue>
#include <string>
#include <xll/log.hpp>

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

using namespace xll;

#define EXPECTED_LOG() \
  ExpectLog exp(__FILE__)
#define ADD_LOG(level, log) \
  addLog(XLL_LOG_CATEGORY_G, level, log)

class ExpectLog : public log::Handler
{
public:
  ExpectLog(const char* file)
    : _file(file)
  {
    log::setHandler(this);
  }

  ~ExpectLog()
  {
    log::setHandler(nullptr);

    if (!_expected.empty())
    {
      CAPTURE(_expected.front().message);
      CHECK(!"message missed");
    }
  }

  void addLog(const char* category, int level, std::string line)
  {
    _expected.push(Expectation{ level, category, line + '\n' });
  }

  void beginLog(
      int level,
      const char* category,
      const char* file,
      unsigned int line) override
  {
    CHECK(_file == file);

    REQUIRE(!_expected.empty());

    CHECK(_expected.front().level == level);
    CHECK(_expected.front().category == category);
  }
  void feed(const char* s, std::size_t n) override
  {
    _current.append(s, n);
  }
  void endLog() override
  {
    CHECK(_expected.front().message == _current);
    _current.clear();
    _expected.pop();
  }

private:
  struct Expectation
  {
    int level;
    const char* category;
    std::string message;
  };

  const char* _file;
  std::string _current;
  std::queue<Expectation> _expected;
};

TEST_CASE("simple log")
{
  log::setLevel(log::LEVEL_DEBUG);
  XLL_LOG_CATEGORY("mycategory");
  EXPECTED_LOG();
  exp.ADD_LOG(log::LEVEL_INFO, "simple test");
  xInf("simple test");
}

TEST_CASE("all levels")
{
  log::setLevel(log::LEVEL_DEBUG);
  XLL_LOG_CATEGORY("mycategory");
  EXPECTED_LOG();
  exp.ADD_LOG(log::LEVEL_DEBUG, "debug log");
  exp.ADD_LOG(log::LEVEL_VERBOSE, "verbose log");
  exp.ADD_LOG(log::LEVEL_INFO, "info log");
  exp.ADD_LOG(log::LEVEL_WARNING, "warning log");
  exp.ADD_LOG(log::LEVEL_ERROR, "error log");
  exp.ADD_LOG(log::LEVEL_FATAL, "fatal log");
  xDeb("debug log");
  xVer("verbose log");
  xInf("info log");
  xWar("warning log");
  xErr("error log");
  xFat("fatal log");
}

TEST_CASE("multiple categories")
{
  log::setLevel(log::LEVEL_DEBUG);
  XLL_LOG_CATEGORY("cat1");
  EXPECTED_LOG();
  exp.addLog("cat1", log::LEVEL_DEBUG, "logging in cat1");
  exp.addLog("cat2", log::LEVEL_DEBUG, "logging in cat2");
  xDeb("logging in cat1");
  xDebC("cat2", "logging in cat2");
}

TEST_CASE("formatting")
{
  log::setLevel(log::LEVEL_DEBUG);
  EXPECTED_LOG();
  exp.addLog("cat", log::LEVEL_DEBUG, "1 + 1 = 2");
  xDebC("cat", "%d + %c = %s", 1, '1', "2");
}

TEST_CASE("level")
{
  log::setLevel(log::LEVEL_INFO);
  XLL_LOG_CATEGORY("cat");
  EXPECTED_LOG();
  exp.ADD_LOG(log::LEVEL_INFO, "info log");
  exp.ADD_LOG(log::LEVEL_DEBUG, "debug log 2");
  xDeb("debug log");
  xInf("info log");
  log::setLevel(log::LEVEL_DEBUG);
  xDeb("debug log 2");
}

TEST_CASE("category filter")
{
  log::setLevel(log::LEVEL_INFO);
  log::addFilter("cat2", log::LEVEL_DEBUG);
  EXPECTED_LOG();
  exp.addLog("cat", log::LEVEL_INFO, "info log");
  exp.addLog("cat2", log::LEVEL_DEBUG, "debug log 2");
  xDebC("cat", "debug log");
  xInfC("cat", "info log");
  xDebC("cat2", "debug log 2");
}
