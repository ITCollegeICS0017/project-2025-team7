// ConsoleUI.hpp
#pragma once
#include "Types.hpp" // NEW: Include definition for Ticket

class ConsoleUI {
public:
    static void showTicket(const Ticket& t);
    static void showMessage(const std::string& msg);
};