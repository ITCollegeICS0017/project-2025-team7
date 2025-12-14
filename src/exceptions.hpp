#pragma once
#include <stdexcept>
#include <string>

// Base exception for the ticketing domain.
class TicketException : public std::runtime_error {
public:
    explicit TicketException(const std::string& message)
        : std::runtime_error(message) {}
};

// Thrown when user input in the UI/Logic layer is syntactically incorrect.
class InvalidInputException : public TicketException {
public:
    explicit InvalidInputException(const std::string& message)
        : TicketException(message) {}
};

// Thrown when logic-level preconditions are violated (wrong state, bad argument value).
class InvalidOperationException : public TicketException {
public:
    explicit InvalidOperationException(const std::string& message)
        : TicketException(message) {}
};

// Thrown when a ticket with the given id is not found in the repository.
class TicketNotFoundException : public TicketException {
public:
    explicit TicketNotFoundException(int id)
        : TicketException("Ticket with id " + std::to_string(id) + " not found.") {}
};


class DataSourceException : public TicketException {
public:
    explicit DataSourceException(const std::string& message)
        : TicketException(message) {}
};
