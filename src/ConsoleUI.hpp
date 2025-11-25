#pragma once
#include "Types.hpp"
#include <string>

class ConsoleUI {
public:
    static void showTicket(const Ticket& t);
    static void showMessage(const std::string& msg);
    static void showError(const std::string& msg);
};
