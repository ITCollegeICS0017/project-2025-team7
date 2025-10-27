// Cashier.cpp
#include "Cashier.hpp"
#include "ConsoleUI.hpp"
#include <iostream>
#include <ctime>
#include <cmath>
#include <string>

Cashier::Cashier(const std::string& id, RailwayConnectionDatabase* db, IClock* clk)
    : Employee(id), db(db), clock(clk) {}

// IMPLEMENTATION ADDED: searchTickets
void Cashier::searchTickets(const std::string& dest) {
    ConsoleUI::showMessage("[Cashier] Searching for available tickets to: " + dest);
    std::vector<Ticket> results = db->search(dest);
    if (results.empty()) {
        ConsoleUI::showMessage("No available tickets found.");
        return;
    }
    for (const auto& t : results) {
        ConsoleUI::showTicket(t);
    }
}

// IMPLEMENTATION ADDED: blockTicket
bool Cashier::blockTicket(int ticketId) {
    Ticket* t = db->getTicketById(ticketId);
    if (!t || t->status != TicketStatus::AVAILABLE) {
        ConsoleUI::showMessage("Cannot block ticket " + std::to_string(ticketId) + ". It's not AVAILABLE.");
        return false;
    }
    db->updateTicketStatus(ticketId, TicketStatus::BLOCKED);
    ConsoleUI::showMessage("Ticket " + std::to_string(ticketId) + " BLOCKED.");
    return true;
}

bool Cashier::sellTicket(int ticketId, const Passenger& p) {
    Ticket* t = db->getTicketById(ticketId);
    if (!t || t->status != TicketStatus::BLOCKED) {
        // Validation: Must be BLOCKED to sell
        ConsoleUI::showMessage("Ticket must be BLOCKED before selling.");
        return false;
    }

    db->updateTicketStatus(ticketId, TicketStatus::SOLD); // Use DB update
    // t->status = TicketStatus::SOLD; // Removed direct access
    soldTickets.push_back(ticketId);
    ConsoleUI::showMessage("Ticket sold to: " + p.name);
    return true;
}

bool Cashier::returnTicket(int ticketId, std::time_t travelDate) {
    Ticket* t = db->getTicketById(ticketId);
    if (!t || t->status != TicketStatus::SOLD) {
        // Validation: Must be SOLD to return
        ConsoleUI::showMessage("Cannot return unsold ticket.");
        return false;
    }

    std::time_t now = clock->now();
    // Calculate days difference:
    // (std::difftime returns a double in seconds) / (seconds in a day)
    double secondsBefore = std::difftime(travelDate, now);
    double daysBefore = secondsBefore / (60.0 * 60.0 * 24.0);
    double penaltyRate = 0.0;

    // Reporting and Penalties: Based on days before travel
    if (daysBefore < 0) { // Safety check for past travel date
        ConsoleUI::showMessage("Cannot return ticket for travel date in the past.");
        return false;
    } else if (daysBefore >= 30) penaltyRate = 0.01;
    else if (daysBefore >= 15) penaltyRate = 0.05;
    else if (daysBefore >= 3) penaltyRate = 0.10;
    else penaltyRate = 0.30;

    double refund = t->cost * (1.0 - penaltyRate);
    db->updateTicketStatus(ticketId, TicketStatus::RETURNED); // Use DB update
    // t->status = TicketStatus::RETURNED; // Removed direct access
    returnedTickets.push_back(ticketId);

    ConsoleUI::showMessage("Refund: " + std::to_string(refund) + " (Penalty Rate: " + std::to_string(penaltyRate * 100) + "%)");
    return true;
}

void Cashier::generateReport() const {
    ConsoleUI::showMessage("[Report for Cashier " + id + " - " + clock->now_str() + "]");
    ConsoleUI::showMessage("Sold tickets: " + std::to_string(soldTickets.size()));
    ConsoleUI::showMessage("Returned tickets: " + std::to_string(returnedTickets.size()));
}