#pragma once
#include <string>

enum class CoachType { SLEEPER, COMPARTMENT, SEATER };
enum class TicketStatus { AVAILABLE, BLOCKED, SOLD, RETURNED };

struct Passenger {
    std::string name;
    std::string socialSecurityCode;
};

struct Ticket {
    int id{};
    std::string destination;
    std::string originStation;
    CoachType coachType{ CoachType::SEATER };
    double cost{ 0.0 };
    TicketStatus status{ TicketStatus::AVAILABLE };
};