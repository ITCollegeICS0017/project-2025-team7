#pragma once
#include "Employee.hpp"
#include "RailwayConnectionDatabase.hpp"
#include "Types.hpp"
#include "IClock.hpp"
#include <vector>

class Cashier : public Employee {
private:
    RailwayConnectionDatabase* db;
    IClock* clock;
    std::vector<int> soldTickets;
    std::vector<int> returnedTickets;

    double computePenaltyRate(int daysBefore) const;

public:
    Cashier(const std::string& id, RailwayConnectionDatabase* db, IClock* clk);

    void searchTickets(const std::string& dest);
    bool blockTicket(int ticketId);
    bool sellTicket(int ticketId, const Passenger& p);
    bool returnTicket(int ticketId, std::time_t travelDate);
    void generateReport() const;
};