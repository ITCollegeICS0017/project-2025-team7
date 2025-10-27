// RailwayConnectionDatabase.hpp
#pragma once
#include "Types.hpp" // NEW: Include definitions
#include <vector>
#include <string>

class RailwayConnectionDatabase {
private:
    std::vector<Ticket> tickets;

public:
    void addTicket(const Ticket& t);
    Ticket* getTicketById(int id);
    std::vector<Ticket> search(const std::string& dest); // Returns list of matching tickets
    bool updateTicketStatus(int id, TicketStatus newStatus);
};