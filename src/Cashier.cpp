#include "Cashier.hpp"
#include "ConsoleUI.hpp"
#include "exceptions.hpp"

#include <cmath>
#include <ctime>
#include <string>

Cashier::Cashier(const std::string& id, RailwayConnectionDatabase* db, IClock* clk)
    : Employee(id), db(db), clock(clk) {}

double Cashier::computePenaltyRate(int daysBefore) const {
    if (daysBefore >= 30)       return 0.01; // 1% if returned one month before
    else if (daysBefore >= 15)  return 0.05; // 5% if returned 15 days before
    else if (daysBefore >= 3)   return 0.10; // 10% if returned 3 days before
    else if (daysBefore >= 0)   return 0.30; // 30% if returned on the day of travel
    throw InvalidOperationException("Cannot return a ticket after departure time.");
}

void Cashier::searchTickets(const std::string& destination, CoachType* coachFilter, std::time_t* dateFilter) {
    if (destination.empty()) {
        throw InvalidInputException("Destination must not be empty.");
    }

    ConsoleUI::showMessage("[Cashier] Searching AVAILABLE tickets to: " + destination);
    auto results = db->search(destination, coachFilter, dateFilter);

    if (results.empty()) {
        ConsoleUI::showMessage("No AVAILABLE tickets found.");
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

    Ticket& t = db->requireTicketById(ticketId);

    if (t.status != TicketStatus::AVAILABLE) {
        throw InvalidOperationException("Only AVAILABLE tickets can be blocked.");
    }

    db->updateTicketStatus(ticketId, TicketStatus::BLOCKED);
    ConsoleUI::showMessage("Ticket #" + std::to_string(ticketId) + " has been BLOCKED (temporarily reserved).");
    return true;
}

bool Cashier::sellTicket(int ticketId, const Passenger& passenger) {
    if (ticketId <= 0) {
        throw InvalidInputException("Ticket id must be a positive integer.");
    }
    if (passenger.surname.empty() || passenger.passportNumber.empty()) {
        throw InvalidInputException("Passport data is required: surname and passport number.");
    }

    Ticket& t = db->requireTicketById(ticketId);
    if (t.status != TicketStatus::BLOCKED) {
        throw InvalidOperationException("Only BLOCKED tickets can be sold.");
    }

    t.status = TicketStatus::SOLD;
    t.passenger = passenger;

    soldTickets.push_back(ticketId);
    totalSales += t.cost;

    ConsoleUI::showMessage(
        "Ticket #" + std::to_string(ticketId) +
        " SOLD to " + passenger.surname +
        ". Payment accepted: " + std::to_string(t.cost)
    );
    return true;
}

bool Cashier::returnTicket(int ticketId) {
    if (ticketId <= 0) {
        throw InvalidInputException("Ticket id must be a positive integer.");
    }

    Ticket& t = db->requireTicketById(ticketId);
    if (t.status != TicketStatus::SOLD) {
        throw InvalidOperationException("Only SOLD tickets can be returned.");
    }

    std::time_t now = clock->now();
    long secondsDiff = static_cast<long>(std::difftime(t.travelDate, now));
    int daysBefore = static_cast<int>(std::floor(secondsDiff / (60.0 * 60.0 * 24.0)));

    double penaltyRate = computePenaltyRate(daysBefore);
    double refund = t.cost * (1.0 - penaltyRate);

    // Mark available again (per requirements) and clear passenger data.
    t.status = TicketStatus::AVAILABLE;
    t.passenger = Passenger{};

    returnedTickets.push_back(ticketId);
    totalRefunds += refund;

    ConsoleUI::showMessage(
        "Ticket #" + std::to_string(ticketId) +
        " RETURNED. Refund: " + std::to_string(refund) +
        " (Penalty: " + std::to_string(penaltyRate * 100.0) + "%)."
    );
    return true;
}

void Cashier::generateDailyReport() const {
    ConsoleUI::showMessage("\n=== Daily Cashier Report (" + id + ") ===");
    ConsoleUI::showMessage("Timestamp: " + clock->now_str());
    ConsoleUI::showMessage("Sold tickets: " + std::to_string(soldTickets.size()));
    ConsoleUI::showMessage("Returned tickets: " + std::to_string(returnedTickets.size()));
    ConsoleUI::showMessage("Gross sales: " + std::to_string(totalSales));
    ConsoleUI::showMessage("Refunds paid: " + std::to_string(totalRefunds));
    ConsoleUI::showMessage("Net revenue: " + std::to_string(totalSales - totalRefunds));
    ConsoleUI::showMessage("Report submitted to the central office.\n");
}
