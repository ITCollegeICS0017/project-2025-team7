#include "Cashier.hpp"
#include "ConsoleUI.hpp"
#include "Exceptions.hpp"
#include <cmath>
#include <ctime>
#include <string>

Cashier::Cashier(const std::string& id, RailwayConnectionDatabase* db, IClock* clk)
    : Employee(id), db(db), clock(clk) {}

double Cashier::computePenaltyRate(int daysBefore) const {
    if (daysBefore >= 30)       return 0.01; // 1%
    else if (daysBefore >= 15)  return 0.05; // 5%
    else if (daysBefore >= 3)   return 0.10; // 10%
    else if (daysBefore >= 0)   return 0.30; // 30% on the day of travel
    throw InvalidOperationException("Cannot return a ticket after departure time.");
}

void Cashier::searchTickets(const std::string& dest) {
    if (dest.empty()) {
        throw InvalidInputException("Destination must not be empty.");
    }

    ConsoleUI::showMessage("[Cashier] Searching for available tickets to: " + dest);
    std::vector<Ticket> results = db->search(dest);

    if (results.empty()) {
        ConsoleUI::showMessage("No AVAILABLE tickets found for destination: " + dest);
        return;
    }

    ConsoleUI::showMessage("Found " + std::to_string(results.size()) + " ticket(s):");
    for (const auto& t : results) {
        ConsoleUI::showTicket(t);
    }
}

bool Cashier::blockTicket(int ticketId) {
    if (ticketId <= 0) {
        throw InvalidInputException("Ticket id must be a positive integer.");
    }

    Ticket& t = db->requireTicketById(ticketId); // may throw TicketNotFoundException

    if (t.status != TicketStatus::AVAILABLE) {
        throw InvalidOperationException("Only AVAILABLE tickets can be blocked.");
    }

    db->updateTicketStatus(ticketId, TicketStatus::BLOCKED);
    ConsoleUI::showMessage("Ticket #" + std::to_string(ticketId) + " has been BLOCKED.");
    return true;
}

bool Cashier::sellTicket(int ticketId, const Passenger& p) {
    if (ticketId <= 0) {
        throw InvalidInputException("Ticket id must be a positive integer.");
    }
    if (p.name.empty() || p.socialSecurityCode.empty()) {
        throw InvalidInputException("Passenger name and social security code must be provided.");
    }

    Ticket& t = db->requireTicketById(ticketId); // may throw TicketNotFoundException

    if (t.status != TicketStatus::BLOCKED) {
        throw InvalidOperationException("Only BLOCKED tickets can be sold.");
    }

    db->updateTicketStatus(ticketId, TicketStatus::SOLD);
    soldTickets.push_back(ticketId);

    ConsoleUI::showMessage("Ticket #" + std::to_string(ticketId) + " has been SOLD to " + p.name + ".");
    return true;
}

bool Cashier::returnTicket(int ticketId, std::time_t travelDate) {
    if (ticketId <= 0) {
        throw InvalidInputException("Ticket id must be a positive integer.");
    }
    if (travelDate == 0) {
        throw InvalidInputException("Travel date must be provided.");
    }

    Ticket& t = db->requireTicketById(ticketId); // may throw TicketNotFoundException

    if (t.status != TicketStatus::SOLD) {
        throw InvalidOperationException("Only SOLD tickets can be returned.");
    }

    std::time_t now = clock->now();
    long secondsDiff = static_cast<long>(difftime(travelDate, now));
    int daysBefore = static_cast<int>(std::floor(secondsDiff / (60.0 * 60.0 * 24.0)));

    double penaltyRate = computePenaltyRate(daysBefore);
    double refund = t.cost * (1.0 - penaltyRate);

    db->updateTicketStatus(ticketId, TicketStatus::RETURNED);
    returnedTickets.push_back(ticketId);

    ConsoleUI::showMessage(
        "Ticket #" + std::to_string(ticketId) +
        " returned. Refund: " + std::to_string(refund) +
        " (Penalty: " + std::to_string(penaltyRate * 100.0) + "%)."
    );

    return true;
}

void Cashier::generateReport() const {
    ConsoleUI::showMessage("[Report for Cashier " + id + " - " + clock->now_str() + "]");
    ConsoleUI::showMessage("Sold tickets: " + std::to_string(soldTickets.size()));
    ConsoleUI::showMessage("Returned tickets: " + std::to_string(returnedTickets.size()));
}