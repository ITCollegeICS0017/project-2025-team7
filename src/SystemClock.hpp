// SystemClock.hpp
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
        std::stringstream ss;
        // Format: YYYY-MM-DD HH:MM:SS
        ss << std::put_time(std::localtime(&t), "%Y-%m-%d %H:%M:%S"); 
        return ss.str();
    }
};