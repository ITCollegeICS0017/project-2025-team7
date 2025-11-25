#include "Types.hpp"
#include "Employee.hpp"
#include "IClock.hpp"
#include "SystemClock.hpp"
#include "Exceptions.hpp"
#include "RailwayConnectionDatabase.hpp"
#include "Cashier.hpp"
#include "ConsoleUI.hpp"

#include <iostream>
#include <limits>

int main() {
    RailwayConnectionDatabase db;
    SystemClock clock;
    Cashier cashier("C001", &db, &clock);

    Ticket t1{ 1, "Tallinn",  "Tartu",  CoachType::SEATER,      25.0, TicketStatus::AVAILABLE };
    Ticket t2{ 2, "Tallinn",  "Narva",  CoachType::COMPARTMENT, 40.0, TicketStatus::AVAILABLE };
    Ticket t3{ 3, "Viljandi", "Tallinn",CoachType::SLEEPER,     60.0, TicketStatus::AVAILABLE };

    db.addTicket(t1);
    db.addTicket(t2);
    db.addTicket(t3);

    ConsoleUI::showMessage("Release 3 Demo: Validation & Exceptions");

    try {
        // 1. Normal search scenario.
        cashier.searchTickets("Tallinn");

        // 2. Block and sell ticket #2.
        cashier.blockTicket(2);

        Passenger passenger{ "Alice Example", "123456-7890" };
        cashier.sellTicket(2, passenger);

        // 3. Process a normal return for ticket #2 ten days before travel.
        std::time_t travelDate = clock.now() + 10 * 24 * 60 * 60; // 10 days in future
        cashier.returnTicket(2, travelDate);

        // 4. Try an invalid return to demonstrate error handling.
        //    Ticket #3 is still AVAILABLE, so returning it should fail.
        ConsoleUI::showMessage("\n--- Attempting invalid return of AVAILABLE ticket (#3) ---");
        cashier.returnTicket(3, travelDate); // Will throw

    }
    catch (const TicketException& ex) {
        ConsoleUI::showError(ex.what());
    }
    catch (const std::exception& ex) {
        ConsoleUI::showError(std::string("Unexpected error: ") + ex.what());
    }

    // 5. Daily report.
    cashier.generateReport();

    ConsoleUI::showMessage("Press ENTER to exit.");
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    return 0;
}
