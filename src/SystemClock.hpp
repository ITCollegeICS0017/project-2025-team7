#pragma once
#include "IClock.hpp"
#include <ctime>
#include <string>
#include <sstream>
#include <iomanip>

class SystemClock : public IClock {
public:
    std::time_t now() const override {
        return std::time(nullptr);
    }

    std::string now_str() const override {
        std::time_t t = now();
        std::tm* tmPtr = std::localtime(&t);

        std::stringstream ss;
        // Format: YYYY-MM-DD HH:MM:SS
        if (tmPtr) {
            ss << std::put_time(tmPtr, "%Y-%m-%d %H:%M:%S");
        }
        return ss.str();
    }
};
