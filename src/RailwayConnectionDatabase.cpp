// RailwayConnectionDatabase.cpp
#include "RailwayConnectionDatabase.hpp"
#include <iostream>

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

std::vector<Ticket> RailwayConnectionDatabase::search(const std::string& dest) {
    std::vector<Ticket> results;
    for (const auto& t : tickets) {
        if (t.destination == dest && t.status == TicketStatus::AVAILABLE) {
            results.push_back(t);
        }
    }
    return results;
}

bool RailwayConnectionDatabase::updateTicketStatus(int id, TicketStatus newStatus) {
    Ticket* t = getTicketById(id);
    if (t) {
        t->status = newStatus;
        return true;
    }
    return false;
}