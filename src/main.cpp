#include <iostream>
#include <string>
#include <vector>
using namespace std;

enum class CoachType { SLEEPER, COMPARTMENT, SEATER };
enum class TicketStatus { AVAILABLE, BLOCKED, SOLD, RETURNED };

struct Passenger {
    string name;
    string socialSecurityCode;
};

struct Ticket {
    int id;
    string destination;
    string originStation;
    CoachType coachType;
    double cost;
    TicketStatus status;
};

class RailwayConnectionDatabase {
public:
    vector<Ticket> tickets;

    void addTicket(const Ticket& t) { tickets.push_back(t); }

    void search(const string& dest) {
        cout << "[DB] Searching tickets for destination: " << dest << endl;
        for (auto& t : tickets) {
            if (t.destination == dest && t.status == TicketStatus::AVAILABLE) {
                cout << " Ticket #" << t.id
                    << " from " << t.originStation
                    << " to " << t.destination
                    << " | Price: " << t.cost << endl;
            }
        }
    }

    void updateStatus(int ticketId, TicketStatus newStatus) {
        for (auto& t : tickets) {
            if (t.id == ticketId) {
                t.status = newStatus;
                cout << "[DB] Ticket " << ticketId << " updated to new status." << endl;
                return;
            }
        }
    }
};

class Cashier {
public:
    string id;
    RailwayConnectionDatabase* db;

    void search_tickets(const string& dest) {
        cout << "[Cashier] Checking availability for: " << dest << endl;
        db->search(dest);
    }

    void block_ticket(int ticketId) {
        cout << "[Cashier] Blocking ticket " << ticketId << endl;
        db->updateStatus(ticketId, TicketStatus::BLOCKED);
    }

    void sell_ticket(int ticketId, Passenger p) {
        cout << "[Cashier] Selling ticket " << ticketId << " to passenger: " << p.name << endl;
        db->updateStatus(ticketId, TicketStatus::SOLD);
    }

    void return_ticket(int ticketId) {
        cout << "[Cashier] Returning ticket " << ticketId << "..." << endl;
        db->updateStatus(ticketId, TicketStatus::RETURNED);
        cout << "[Cashier] Refund processed (minus penalty)" << endl;
    }

    void generate_report() {
        cout << "[Cashier] Generating daily report..." << endl;
    }
};

int main() {
    RailwayConnectionDatabase db;

    // Add sample tickets
    db.addTicket({ 1, "Tallinn", "Tartu", CoachType::SLEEPER, 50.0, TicketStatus::AVAILABLE });
    db.addTicket({ 2, "Tartu", "Tallinn", CoachType::SEATER, 30.0, TicketStatus::AVAILABLE });

    Cashier cashier{ "C1", &db };
    Passenger passenger{ "Kamil", "123456789" };

    // User Story: Cashier sells a blocked ticket
    cashier.search_tickets("Tallinn");
    cashier.block_ticket(1);
    cashier.sell_ticket(1, passenger);
    system("pause");

    return 0;
}