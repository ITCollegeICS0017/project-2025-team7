#include "ConsoleUI.hpp"

#include <iostream>
#include <iomanip>
#include <ctime>
#include <sstream>

namespace {
    const char* statusToString(TicketStatus s) {
        switch (s) {
            case TicketStatus::AVAILABLE: return "AVAILABLE";
            case TicketStatus::BLOCKED:   return "BLOCKED";
            case TicketStatus::SOLD:      return "SOLD";
        }
        return "UNKNOWN";
    }

    const char* coachToString(CoachType c) {
        switch (c) {
            case CoachType::SLEEPER:     return "SLEEPER";
            case CoachType::COMPARTMENT: return "COMPARTMENT";
            case CoachType::SEATER:      return "SEATER";
        }
        return "UNKNOWN";
    }

    std::string dateToString(std::time_t t) {
        std::tm* tmPtr = std::localtime(&t);
        if (!tmPtr) return "N/A";
        std::ostringstream oss;
        oss << std::put_time(tmPtr, "%Y-%m-%d");
        return oss.str();
    }
}

void ConsoleUI::showTicket(const Ticket& t) {
    std::cout
        << "Ticket #" << t.id
        << " | " << t.originStation << " -> " << t.destination
        << " | Date: " << dateToString(t.travelDate)
        << " | Coach: " << coachToString(t.coachType)
        << " | Price: " << std::fixed << std::setprecision(2) << t.cost
        << " | Status: " << statusToString(t.status);

    if (t.status == TicketStatus::SOLD) {
        std::cout << " | Passenger: " << t.passenger.surname
                  << " (" << t.passenger.passportNumber << ")";
    }
    std::cout << "\n";
}

void ConsoleUI::showMessage(const std::string& msg) {
    std::cout << msg << "\n";
}

void ConsoleUI::showError(const std::string& msg) {
    std::cout << "[ERROR] " << msg << "\n";
}
