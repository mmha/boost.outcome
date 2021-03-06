#include "../../include/boost/outcome.hpp"

using namespace boost::outcome;
extern outcome<int> unknown();
extern BOOSTLITE_NOINLINE outcome<int> test1()
{
  return unknown().next([](outcome<int> m) { return m.get()*3; });
}
extern BOOSTLITE_NOINLINE void test2()
{
}

int main(void)
{
  outcome<int> m(test1());
  test2();
  return 0;
}
