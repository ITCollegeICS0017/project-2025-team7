// IClock.hpp
#pragma once
#include <chrono>
#include <ctime>
#include <string> // Need string for now_str() return type

class IClock {
public:
    virtual std::time_t now() const = 0;
    // NEW: Function to get formatted time string for reporting
    virtual std::string now_str() const = 0; 
    virtual ~IClock() = default;
};