#include "ConsoleUI.hpp"
#include <iostream>

namespace {
    const char* statusToString(TicketStatus s) {
        switch (s) {
        case TicketStatus::AVAILABLE: return "AVAILABLE";
        case TicketStatus::BLOCKED:   return "BLOCKED";
        case TicketStatus::SOLD:      return "SOLD";
        case TicketStatus::RETURNED:  return "RETURNED";
        default:                      return "UNKNOWN";
        }
    }

    const char* coachToString(CoachType c) {
        switch (c) {
        case CoachType::SLEEPER:     return "SLEEPER";
        case CoachType::COMPARTMENT: return "COMPARTMENT";
        case CoachType::SEATER:      return "SEATER";
        default:                     return "UNKNOWN";
        }
    }
}

void ConsoleUI::showTicket(const Ticket& t) {
    std::cout << "Ticket #" << t.id
        << " [" << coachToString(t.coachType) << "] "
        << t.originStation << " -> " << t.destination
        << " | Price: " << t.cost
        << " | Status: " << statusToString(t.status)
        << std::endl;
}

void ConsoleUI::showMessage(const std::string& msg) {
    std::cout << msg << std::endl;
}

void ConsoleUI::showError(const std::string& msg) {
    std::cout << "[ERROR] " << msg << std::endl;
}
