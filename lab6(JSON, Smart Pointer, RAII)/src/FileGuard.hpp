#pragma once
#include "Logger.hpp"
#include <string>
#include <fstream>
#include <stdexcept>

// 1-st RAII class (открывает файл при создании, закрывает при уничтожении)
class FileGuard {
public:
    explicit FileGuard(const std::string& path, std::ios::openmode mode = std::ios::in) {
        m_stream.open(path, mode);
        if (!m_stream.is_open()) {
            throw std::runtime_error("Cannot open file: " + path);
        }
        Logger::info("FileGuard: opened " + path);
    }

    ~FileGuard() {
        if (m_stream.is_open()) {
            m_stream.close();
            Logger::info("FileGuard: file closed");
        }
    }

    std::fstream& stream() { return m_stream; }

    FileGuard(const FileGuard&)            = delete;
    FileGuard& operator=(const FileGuard&) = delete;

private:
    std::fstream m_stream;
};
