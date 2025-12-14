#pragma once

#include <string>
#include <ctime>

// --- Domain Enumerations ---
enum class CoachType { SLEEPER, COMPARTMENT, SEATER };

// Ticket can be listed, temporarily blocked for purchase, or sold.
// When a sold ticket is returned, it becomes AVAILABLE again.
enum class TicketStatus { AVAILABLE, BLOCKED, SOLD };

// Minimal passenger/passport data requested by the variant.
struct Passenger {
    std::string surname;
    std::string passportNumber;
};

// One ticket offer in the central database.
struct Ticket {
    int id{};
    std::string originStation;
    std::string destination;
    std::time_t travelDate{};             // seconds since epoch
    CoachType coachType{CoachType::SEATER};
    double cost{0.0};
    TicketStatus status{TicketStatus::AVAILABLE};

    // Filled when SOLD; empty otherwise.
    Passenger passenger{};
};
