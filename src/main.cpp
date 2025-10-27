// main.cpp
#include "RailwayConnectionDatabase.hpp"
#include "RailwayConnectionDatabase.cpp" // NEW: Include implementation
#include "Cashier.hpp"
#include "Cashier.cpp" // NEW: Include implementation
#include "ConsoleUI.cpp" // NEW: Include implementation
#include "SystemClock.hpp" // NEW: Include declaration
#include "Types.hpp" // NEW: Include types

int main() {
    // Necessary Includes:
    // RailwayConnectionDatabase.cpp, Cashier.cpp, ConsoleUI.cpp, SystemClock.hpp, Types.hpp
    
    // --- SETUP ---
    RailwayConnectionDatabase db;
    // Note: Ticket struct now comes from Types.hpp
    db.addTicket({ 1, "Tallinn", "Tartu", CoachType::SLEEPER, 50.0, TicketStatus::AVAILABLE });
    db.addTicket({ 2, "Tartu", "Tallinn", CoachType::SEATER, 30.0, TicketStatus::AVAILABLE });
    db.addTicket({ 3, "Parnu", "Viljandi", CoachType::COMPARTMENT, 80.0, TicketStatus::AVAILABLE });


    SystemClock clock;
    Cashier cashier("C1", &db, &clock);
    Passenger passenger{ "Kamil", "123456789" };

    // --- SCENARIO EXECUTION ---

    // 1. Search and Block
    cashier.searchTickets("Tallinn"); // Shows ticket #2
    cashier.blockTicket(2); // Status: BLOCKED
    
    // 2. Invalid Sell attempt (Ticket #3 is AVAILABLE, not BLOCKED)
    cashier.sellTicket(3, passenger); // Should fail validation

    // 3. Successful Sell
    cashier.sellTicket(2, passenger); // Status: SOLD (Valid, was BLOCKED)

    // 4. Return Ticket (#2) with a future travel date (10 days from now)
    // This will hit the 10% penalty logic (daysBefore >= 3)
    std::time_t futureDate = std::time(nullptr) + 10 * 24 * 60 * 60; // 10 days from now
    cashier.returnTicket(2, futureDate); // Status: RETURNED (Refund calculated)

    // 5. Generate Report
    cashier.generateReport();

    system("pause");
    
    return 0;
}