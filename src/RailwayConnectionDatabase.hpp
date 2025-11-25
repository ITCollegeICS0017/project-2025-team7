// RailwayConnectionDatabase.hpp
#pragma once
#include "Types.hpp"
#include "Exceptions.hpp"
#include <vector>
#include <string>

class RailwayConnectionDatabase {
private:
    std::vector<Ticket> tickets;

public:
    void addTicket(const Ticket& t);

    Ticket* getTicketById(int id);

    Ticket& requireTicketById(int id);

    std::vector<Ticket> search(const std::string& dest) const;

    void updateTicketStatus(int id, TicketStatus newStatus);
};
