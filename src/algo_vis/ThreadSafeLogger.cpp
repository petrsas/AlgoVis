#include "ThreadSafeLogger.hpp"

static void ThreadSafeLogger::log_alloc(const char* name, const size_t count, const size_t size) {
        std::lock_guard<std::mutex> lock(mutex); //auto unlock when out of scope
        logs.emplace_back(std::format("Allocated {} of {} requiring {} bytes\n", count, name, size));
}

static void ThreadSafeLogger::print_log() {
        for (const std::string& l : logs) {
                std::cout<<l;
}

