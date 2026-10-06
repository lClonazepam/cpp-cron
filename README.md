# cpp-cron

Five-field cron matcher and next-run calculator. Supports lists, ranges, and steps.

```cpp
kit::Cron job("0 9 * * 1-5");
std::time_t when = job.next_after(std::time(nullptr));
```

Day-of-month and day-of-week follow classic cron OR semantics when both are restricted. MIT
