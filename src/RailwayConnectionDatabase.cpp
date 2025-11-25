// RailwayConnectionDatabase.cpp
#include "RailwayConnectionDatabase.hpp"

void RailwayConnectionDatabase::addTicket(const Ticket& t) {
    tickets.push_back(t);
}

Ticket* RailwayConnectionDatabase::getTicketById(int id) {
    for (auto& t : tickets) {
        if (t.id == id) {
            return &t;
        }
    }
    return nullptr;
}

Ticket& RailwayConnectionDatabase::requireTicketById(int id) {
    Ticket* ptr = getTicketById(id);
    if (!ptr) {
        throw TicketNotFoundException(id);
    }
    return *ptr;
}

std::vector<Ticket> RailwayConnectionDatabase::search(const std::string& dest) const {
    std::vector<Ticket> results;
    for (const auto& t : tickets) {
        if (t.destination == dest && t.status == TicketStatus::AVAILABLE) {
            results.push_back(t);
        }
    }
    return results;
}

void RailwayConnectionDatabase::updateTicketStatus(int id, TicketStatus newStatus) {
    Ticket& t = requireTicketById(id);  // may throw TicketNotFoundException
    t.status = newStatus;
}
