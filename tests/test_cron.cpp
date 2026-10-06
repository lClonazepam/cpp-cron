#include "cron.hpp"
#include <cassert>
#include <iostream>

int main() {
  kit::Cron every("*/5 * * * *");
  std::tm t{};
  t.tm_min = 10; t.tm_hour = 3; t.tm_mday = 2; t.tm_mon = 0; t.tm_wday = 4; t.tm_year = 126;
  assert(every.matches(t));
  t.tm_min = 11;
  assert(!every.matches(t));
  kit::Cron work("0 9 * * 1-5");
  t.tm_min = 0; t.tm_hour = 9; t.tm_wday = 1;
  assert(work.matches(t));
  t.tm_wday = 0;
  assert(!work.matches(t));
  std::time_t next = every.next_after(1700000000);
  assert(next > 1700000000);
  std::cout << "cron ok next=" << next << "\n";
}
