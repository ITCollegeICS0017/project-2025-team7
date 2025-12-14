#include "RailwayConnectionDatabase.hpp"

#include <fstream>
#include <sstream>
#include <iomanip>
#include <cctype>

// ------------------- helpers for enum conversion -------------------

static std::string coachToString(CoachType c) {
    switch (c) {
        case CoachType::SLEEPER: return "SLEEPER";
        case CoachType::COMPARTMENT: return "COMPARTMENT";
        case CoachType::SEATER: return "SEATER";
    }
    return "SEATER";
}

static bool stringToCoach(const std::string& s, CoachType& out) {
    if (s == "SLEEPER") { out = CoachType::SLEEPER; return true; }
    if (s == "COMPARTMENT") { out = CoachType::COMPARTMENT; return true; }
    if (s == "SEATER") { out = CoachType::SEATER; return true; }
    return false;
}

static std::string statusToString(TicketStatus st) {
    switch (st) {
        case TicketStatus::AVAILABLE: return "AVAILABLE";
        case TicketStatus::BLOCKED: return "BLOCKED";
        case TicketStatus::SOLD: return "SOLD";
    }
    return "AVAILABLE";
}

static bool stringToStatus(const std::string& s, TicketStatus& out) {
    if (s == "AVAILABLE") { out = TicketStatus::AVAILABLE; return true; }
    if (s == "BLOCKED") { out = TicketStatus::BLOCKED; return true; }
    if (s == "SOLD") { out = TicketStatus::SOLD; return true; }
    return false;
}

static std::vector<std::string> split(const std::string& line, char delim) {
    std::vector<std::string> parts;
    std::stringstream ss(line);
    std::string item;
    while (std::getline(ss, item, delim)) {
        parts.push_back(item);
    }
    return parts;
}

static bool isDigits(const std::string& s) {
    if (s.empty()) return false;
    for (char ch : s) {
        if (!std::isdigit(static_cast<unsigned char>(ch)) && ch != '-') return false;
    }
    return true;
}

// ------------------- RailwayConnectionDatabase -------------------

RailwayConnectionDatabase::RailwayConnectionDatabase(std::string file)
    : filePath(std::move(file)) {}

RailwayConnectionDatabase::~RailwayConnectionDatabase() {
    delete[] data;
    data = nullptr;
    count = 0;
    capacity = 0;
}

void RailwayConnectionDatabase::ensureCapacity(int minCapacity) {
    if (capacity >= minCapacity) return;
    int newCap = (capacity == 0) ? 4 : capacity;
    while (newCap < minCapacity) newCap *= 2;

    Ticket* newData = new Ticket[newCap];
    for (int i = 0; i < count; ++i) {
        newData[i] = data[i];
    }
    delete[] data;
    data = newData;
    capacity = newCap;
}

void RailwayConnectionDatabase::addTicket(const Ticket& t) {
    // Invariant: IDs must be unique to prevent double sales.
    if (getTicketById(t.id) != nullptr) {
        throw InvalidOperationException("Ticket id must be unique (id already exists).");
    }
    ensureCapacity(count + 1);
    data[count++] = t;
}

int RailwayConnectionDatabase::size() const {
    return count;
}

const Ticket& RailwayConnectionDatabase::getByIndex(int index) const {
    if (index < 0 || index >= count) throw InvalidOperationException("Index out of range.");
    return data[index];
}

Ticket& RailwayConnectionDatabase::getByIndex(int index) {
    if (index < 0 || index >= count) throw InvalidOperationException("Index out of range.");
    return data[index];
}

Ticket* RailwayConnectionDatabase::getTicketById(int id) {
    for (int i = 0; i < count; ++i) {
        if (data[i].id == id) return &data[i];
    }
    return nullptr;
}

Ticket& RailwayConnectionDatabase::requireTicketById(int id) {
    Ticket* ptr = getTicketById(id);
    if (!ptr) throw TicketNotFoundException(id);
    return *ptr;
}

std::vector<Ticket> RailwayConnectionDatabase::search(const std::string& destination,
                                                      CoachType* coachFilter,
                                                      std::time_t* dateFilter) const {
    std::vector<Ticket> result;
    for (int i = 0; i < count; ++i) {
        const Ticket& t = data[i];
        if (t.destination != destination) continue;
        if (coachFilter && t.coachType != *coachFilter) continue;
        if (dateFilter && t.travelDate != *dateFilter) continue;
        if (t.status != TicketStatus::AVAILABLE) continue; // only offer available tickets
        result.push_back(t);
    }
    return result;
}

void RailwayConnectionDatabase::updateTicketStatus(int id, TicketStatus newStatus) {
    Ticket& t = requireTicketById(id);
    t.status = newStatus;
}

bool RailwayConnectionDatabase::parseLine(const std::string& line, Ticket& outTicket) {
    // File format (9 fields):
    // id;origin;destination;travelDateEpoch;coachType;cost;status;passengerSurname;passportNumber
    auto parts = split(line, ';');
    if (parts.size() != 9) return false;

    // id
    if (!isDigits(parts[0])) return false;
    int id = 0;
    try { id = std::stoi(parts[0]); } catch (...) { return false; }
    if (id <= 0) return false;

    // travelDate
    if (!isDigits(parts[3])) return false;
    std::time_t date = 0;
    try { date = static_cast<std::time_t>(std::stoll(parts[3])); } catch (...) { return false; }
    if (date < 0) return false;

    // coach
    CoachType coach{};
    if (!stringToCoach(parts[4], coach)) return false;

    // cost
    double cost = 0.0;
    try { cost = std::stod(parts[5]); } catch (...) { return false; }
    if (cost < 0.0) return false;

    // status
    TicketStatus status{};
    if (!stringToStatus(parts[6], status)) return false;

    outTicket.id = id;
    outTicket.originStation = parts[1];
    outTicket.destination = parts[2];
    outTicket.travelDate = date;
    outTicket.coachType = coach;
    outTicket.cost = cost;
    outTicket.status = status;
    outTicket.passenger.surname = parts[7];
    outTicket.passenger.passportNumber = parts[8];

    // Consistency: passenger data must be empty unless SOLD.
    if (outTicket.status != TicketStatus::SOLD) {
        outTicket.passenger = Passenger{};
    } else {
        if (outTicket.passenger.surname.empty() || outTicket.passenger.passportNumber.empty()) return false;
    }

    return true;
}

std::string RailwayConnectionDatabase::toLine(const Ticket& t) {
    std::ostringstream oss;
    oss << t.id << ';'
        << t.originStation << ';'
        << t.destination << ';'
        << static_cast<long long>(t.travelDate) << ';'
        << coachToString(t.coachType) << ';'
        << std::fixed << std::setprecision(2) << t.cost << ';'
        << statusToString(t.status) << ';';

    if (t.status == TicketStatus::SOLD) {
        oss << t.passenger.surname << ';' << t.passenger.passportNumber;
    } else {
        oss << ';';
    }
    return oss.str();
}

void RailwayConnectionDatabase::loadAll() {
    // Reset memory first.
    delete[] data;
    data = nullptr;
    count = 0;
    capacity = 0;

    std::ifstream in(filePath);
    if (!in.is_open()) {
        // Requirement: if file does not exist, start with an empty repository.
        return;
    }

    std::string line;
    while (std::getline(in, line)) {
        if (line.empty()) continue;
        Ticket t;
        if (!parseLine(line, t)) {
            // Requirement: malformed lines are ignored and must not terminate program.
            continue;
        }
        // During load, avoid exceptions from duplicate IDs caused by corrupted file.
        if (getTicketById(t.id) != nullptr) continue;
        ensureCapacity(count + 1);
        data[count++] = t;
    }
}

void RailwayConnectionDatabase::saveAll() const {
    std::ofstream out(filePath, std::ios::trunc);
    if (!out.is_open()) {
        // For this course, file-open problems are considered a data-source error.
        throw DataSourceException("Cannot open data file for writing: " + filePath);
    }

    for (int i = 0; i < count; ++i) {
        out << toLine(data[i]) << "\n";
    }
}
