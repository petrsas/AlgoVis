#pragma once
#include <vector>
#include <mutex>


class ThreadSafeLogger {
//most likely
    static inline std::vector<std::string> logs;
    static inline std::mutex mutex;
public:
    static void log_alloc(const char* name, const size_t count, const size_t size);
    static void print_log();
};
