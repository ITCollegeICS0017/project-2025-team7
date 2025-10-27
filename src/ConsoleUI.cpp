// ConsoleUI.cpp
#include "ConsoleUI.hpp"
#include <iostream>

void ConsoleUI::showTicket(const Ticket& t) {
    // Added coach type for better reporting
    std::cout << "Ticket #" << t.id << " from " << t.originStation
        << " to " << t.destination << " | Price: " << t.cost 
        << " | Status: " << (t.status == TicketStatus::AVAILABLE ? "AVAILABLE" : 
                            (t.status == TicketStatus::BLOCKED ? "BLOCKED" : 
                             (t.status == TicketStatus::SOLD ? "SOLD" : "RETURNED")))
        << std::endl;
}

void ConsoleUI::showMessage(const std::string& msg) {
    std::cout << msg << std::endl;
}