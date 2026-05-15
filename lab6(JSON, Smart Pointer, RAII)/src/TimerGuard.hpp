#pragma once
#include "Logger.hpp"
#include <string>
#include <chrono>

// 2-nd RAII class (запоминает время при создании, выводит сколько прошло при уничтожении)
class TimerGuard {
public:
    explicit TimerGuard(const std::string& label)
        : m_label(label)
        , m_start(std::chrono::steady_clock::now())
    {
        Logger::info("TimerGuard: [" + m_label + "] started");
    }

    ~TimerGuard() {
        auto end  = std::chrono::steady_clock::now();
        auto ms   = std::chrono::duration_cast<std::chrono::milliseconds>(end - m_start).count();
        Logger::info("TimerGuard: [" + m_label + "] finished in " + std::to_string(ms) + " ms");
    }

    TimerGuard(const TimerGuard&)            = delete;
    TimerGuard& operator=(const TimerGuard&) = delete;

private:
    std::string                                  m_label;
    std::chrono::steady_clock::time_point        m_start;
};
