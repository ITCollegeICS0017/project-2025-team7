#pragma once
#include <string>

class Employee {
protected:
    std::string id;
public:
    Employee(const std::string& id) : id(id) {}
    virtual ~Employee() = default;
    std::string getId() const { return id; }
};
