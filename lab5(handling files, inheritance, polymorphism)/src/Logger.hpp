#pragma once
#include <iostream>
#include <string>
#include <ctime>

class Logger {
public:
    static void debug  (const std::string& message) { log("DEBUG",   message); }
    static void info   (const std::string& message) { log("INFO",    message); }
    static void warning(const std::string& message) { log("WARNING", message); }
    static void error  (const std::string& message) { log("ERROR",   message); }

private:
    static std::string getTime() {
        time_t now = time(0);
        char buf[20];
        strftime(buf, sizeof(buf), "%d.%m.%Y %H:%M:%S", localtime(&now));
        return buf;
    }
    static void log(const std::string& level, const std::string& message) {
        std::cout << getTime() << " [" << level << "] " << message << std::endl;
    }
};
