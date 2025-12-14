#pragma once

#include "Types.hpp"
#include "exceptions.hpp"
#include <string>
#include <vector>

// Release 4: Repository + Persistence.
// Central in-memory storage is a dynamic array of Ticket objects.
// Persistent storage is a text file (one ticket per line).
class RailwayConnectionDatabase {
private:
    Ticket* data{nullptr};
    int count{0};
    int capacity{0};
    std::string filePath;

    void ensureCapacity(int minCapacity);
    static bool parseLine(const std::string& line, Ticket& outTicket);
    static std::string toLine(const Ticket& t);

public:
    explicit RailwayConnectionDatabase(std::string file = "data.txt");
    ~RailwayConnectionDatabase();

    RailwayConnectionDatabase(const RailwayConnectionDatabase&) = delete;
    RailwayConnectionDatabase& operator=(const RailwayConnectionDatabase&) = delete;

    // Basic repository operations
    void addTicket(const Ticket& t);
    int size() const;
    const Ticket& getByIndex(int index) const;
    Ticket& getByIndex(int index);

    // Domain-specific helpers
    Ticket* getTicketById(int id);
    Ticket& requireTicketById(int id);
    std::vector<Ticket> search(const std::string& destination,
                              CoachType* coachFilter = nullptr,
                              std::time_t* dateFilter = nullptr) const;
    void updateTicketStatus(int id, TicketStatus newStatus);

    // Release 4 persistence
    void loadAll();   // File -> memory (invalid lines skipped, no exceptions due to file content)
    void saveAll() const; // memory -> file (complete rewrite)
};
