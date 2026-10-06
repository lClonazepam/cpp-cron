#pragma once
#include <ctime>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>

namespace kit {

class Cron {
 public:
  explicit Cron(std::string_view expr) { parse(expr); }

  bool matches(const std::tm& t) const {
    const int dow = t.tm_wday;
    const bool dom = contains(dom_, t.tm_mday);
    const bool dow_ok = contains(dow_, dow) || (dow == 0 && contains(dow_, 7));
    const bool day_ok = star_dom_ && star_dow_ ? true : (!star_dom_ && !star_dow_ ? (dom || dow_ok) : (star_dom_ ? dow_ok : dom));
    return contains(minute_, t.tm_min) && contains(hour_, t.tm_hour) && contains(month_, t.tm_mon + 1) && day_ok;
  }

  std::time_t next_after(std::time_t from) const {
    std::time_t t = from + 60 - (from % 60);
    for (int i = 0; i < 366 * 24 * 60; ++i) {
      std::tm local{};
#if defined(_WIN32)
      localtime_s(&local, &t);
#else
      localtime_r(&t, &local);
#endif
      local.tm_sec = 0;
      if (matches(local)) return t;
      t += 60;
    }
    throw std::runtime_error("no cron match within a year");
  }

 private:
  struct Field { bool any = false; bool bits[64]{}; };
  Field minute_, hour_, dom_, month_, dow_;
  bool star_dom_ = true, star_dow_ = true;
  static bool contains(const Field& f, int v) { return f.any || (v >= 0 && v < 64 && f.bits[v]); }
  static Field parse_field(std::string_view text, int lo, int hi) {
    Field f;
    if (text == "*") { f.any = true; return f; }
    std::stringstream ss{std::string(text)};
    std::string part;
    while (std::getline(ss, part, ',')) {
      int step = 1;
      auto slash = part.find('/');
      if (slash != std::string::npos) { step = std::stoi(part.substr(slash + 1)); part = part.substr(0, slash); }
      int a = lo, b = hi;
      if (part != "*") {
        auto dash = part.find('-');
        if (dash == std::string::npos) a = b = std::stoi(part);
        else { a = std::stoi(part.substr(0, dash)); b = std::stoi(part.substr(dash + 1)); }
      }
      if (step <= 0 || a < lo || b > hi || a > b) throw std::runtime_error("bad cron field");
      for (int v = a; v <= b; v += step) f.bits[v] = true;
    }
    return f;
  }
  void parse(std::string_view expr) {
    std::stringstream ss{std::string(expr)};
    std::string a, b, c, d, e, extra;
    if (!(ss >> a >> b >> c >> d >> e) || (ss >> extra)) throw std::runtime_error("cron needs 5 fields");
    star_dom_ = (c == "*"); star_dow_ = (e == "*");
    minute_ = parse_field(a, 0, 59);
    hour_ = parse_field(b, 0, 23);
    dom_ = parse_field(c, 1, 31);
    month_ = parse_field(d, 1, 12);
    dow_ = parse_field(e, 0, 7);
  }
};

}  // namespace kit
