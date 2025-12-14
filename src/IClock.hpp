#pragma once
#include <ctime>
#include <string>

class IClock {
public:
    virtual std::time_t now() const = 0;
    virtual std::string now_str() const = 0;
    virtual ~IClock() = default;
};
