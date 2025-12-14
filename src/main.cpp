#include "Types.hpp"
#include "Employee.hpp"
#include "IClock.hpp"
#include "SystemClock.hpp"
#include "exceptions.hpp"
#include "RailwayConnectionDatabase.hpp"
#include "Cashier.hpp"
#include "ConsoleUI.hpp"

#include <iostream>
#include <limits>
#include <string>
#include <ctime>

namespace {
    void clearLine() {
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    int readInt(const std::string& prompt) {
        ConsoleUI::showMessage(prompt);
        int v{};
        if (!(std::cin >> v)) {
            std::cin.clear();
            clearLine();
            throw InvalidInputException("Expected an integer value.");
        }
        clearLine();
        return v;
    }

    double readDouble(const std::string& prompt) {
        ConsoleUI::showMessage(prompt);
        double v{};
        if (!(std::cin >> v)) {
            std::cin.clear();
            clearLine();
            throw InvalidInputException("Expected a numeric value.");
        }
        clearLine();
        return v;
    }

    std::string readString(const std::string& prompt) {
        ConsoleUI::showMessage(prompt);
        std::string s;
        std::getline(std::cin, s);
        return s;
    }

    CoachType readCoachType() {
        ConsoleUI::showMessage("Choose coach type: 1) SEATER  2) COMPARTMENT  3) SLEEPER");
        int choice = readInt("Enter choice (1-3):");
        switch (choice) {
            case 1: return CoachType::SEATER;
            case 2: return CoachType::COMPARTMENT;
            case 3: return CoachType::SLEEPER;
            default: throw InvalidInputException("Coach type must be 1..3.");
        }
    }

    std::time_t readDateEpoch(const std::string& prompt) {
        // Simple input: YYYY-MM-DD (local time, midnight)
        std::string s = readString(prompt + " (YYYY-MM-DD):");
        if (s.size() != 10 || s[4] != '-' || s[7] != '-') {
            throw InvalidInputException("Date must be in format YYYY-MM-DD.");
        }
        int y = std::stoi(s.substr(0,4));
        int m = std::stoi(s.substr(5,2));
        int d = std::stoi(s.substr(8,2));
        std::tm tm{};
        tm.tm_year = y - 1900;
        tm.tm_mon = m - 1;
        tm.tm_mday = d;
        tm.tm_hour = 0; tm.tm_min = 0; tm.tm_sec = 0;
        std::time_t t = std::mktime(&tm);
        if (t == -1) throw InvalidInputException("Invalid date value.");
        return t;
    }

    void seedIfEmpty(RailwayConnectionDatabase& db, IClock& clock) {
        if (db.size() > 0) return;

        std::time_t base = clock.now();
        // Add a few sample tickets for the first run.
        db.addTicket(Ticket{1, "Tallinn", "Tartu", base + 7 * 24 * 3600, CoachType::SEATER, 25.0, TicketStatus::AVAILABLE, {}});
        db.addTicket(Ticket{2, "Tallinn", "Narva", base + 10 * 24 * 3600, CoachType::COMPARTMENT, 40.0, TicketStatus::AVAILABLE, {}});
        db.addTicket(Ticket{3, "Viljandi", "Tallinn", base + 20 * 24 * 3600, CoachType::SLEEPER, 60.0, TicketStatus::AVAILABLE, {}});
    }

    void listAll(const RailwayConnectionDatabase& db) {
        ConsoleUI::showMessage("\n--- All tickets in database ---");
        for (int i = 0; i < db.size(); ++i) {
            ConsoleUI::showTicket(db.getByIndex(i));
        }
        ConsoleUI::showMessage("");
    }

    void createTicket(RailwayConnectionDatabase& db) {
        int id = readInt("Ticket id:");
        std::string origin = readString("Origin station:");
        std::string dest = readString("Destination:");
        std::time_t date = readDateEpoch("Travel date");
        CoachType coach = readCoachType();
        double price = readDouble("Ticket price:");
        Ticket t{};
        t.id = id;
        t.originStation = origin;
        t.destination = dest;
        t.travelDate = date;
        t.coachType = coach;
        t.cost = price;
        t.status = TicketStatus::AVAILABLE;
        db.addTicket(t);
        ConsoleUI::showMessage("Ticket added.");
    }
}

int main() {
    RailwayConnectionDatabase db("data.txt");
    SystemClock clock;
    Cashier cashier("C001", &db, &clock);

    try {
        db.loadAll();
        seedIfEmpty(db, clock);
    } catch (const TicketException& ex) {
        ConsoleUI::showError(ex.what());
        // If loadAll threw (it shouldn't for file content), start empty.
    }

    ConsoleUI::showMessage("Railway Ticket Sales System\n");

    bool running = true;
    while (running) {
        ConsoleUI::showMessage("Menu:");
        ConsoleUI::showMessage(" 1) List all tickets");
        ConsoleUI::showMessage(" 2) Search available tickets");
        ConsoleUI::showMessage(" 3) Block ticket (temporary reservation)");
        ConsoleUI::showMessage(" 4) Sell ticket (requires BLOCKED)");
        ConsoleUI::showMessage(" 5) Return ticket (refund with penalty)");
        ConsoleUI::showMessage(" 6) Add new ticket to database");
        ConsoleUI::showMessage(" 7) Daily report");
        ConsoleUI::showMessage(" 0) Exit (save to file)");
        try {
            int choice = readInt("Choose option:");
            switch (choice) {
                case 1: listAll(db); break;
                case 2: {
                    std::string dest = readString("Destination:");
                    // Optional coach filter
                    std::string useCoach = readString("Filter by coach type? (y/n):");
                    CoachType coach{};
                    CoachType* coachPtr = nullptr;
                    if (!useCoach.empty() && (useCoach[0] == 'y' || useCoach[0] == 'Y')) {
                        coach = readCoachType();
                        coachPtr = &coach;
                    }
                    cashier.searchTickets(dest, coachPtr, nullptr);
                    break;
                }
                case 3: {
                    int id = readInt("Ticket id to block:");
                    cashier.blockTicket(id);
                    break;
                }
                case 4: {
                    int id = readInt("Ticket id to sell:");
                    Passenger p{};
                    p.surname = readString("Passenger surname (passport):");
                    p.passportNumber = readString("Passport number:");
                    cashier.sellTicket(id, p);
                    break;
                }
                case 5: {
                    int id = readInt("Ticket id to return:");
                    cashier.returnTicket(id);
                    break;
                }
                case 6: createTicket(db); break;
                case 7: cashier.generateDailyReport(); break;
                case 0:
                    running = false;
                    break;
                default:
                    ConsoleUI::showMessage("Unknown option.");
                    break;
            }
        } catch (const TicketException& ex) {
            ConsoleUI::showError(ex.what());
        } catch (const std::exception& ex) {
            ConsoleUI::showError(std::string("Unexpected error: ") + ex.what());
        }
    }

    try {
        db.saveAll();
        ConsoleUI::showMessage("Data saved to data.txt. Goodbye.");
    } catch (const TicketException& ex) {
        ConsoleUI::showError(ex.what());
        ConsoleUI::showMessage("Program exits without saving.");
    }

    return 0;
}
