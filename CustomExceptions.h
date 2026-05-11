// =====================================================================
//  File:     CustomExceptions.h
//  Project:  GuildForge: Adventurer's Equipment Emporium
//  Course:   CSCI 272 - Object-Oriented Programming in C++
//  Group:    Group 2 - "The Boys"
//  Owner:    Jn Francois, Jodestee
//  Purpose:  Defines the custom exception classes the rest of the program
//            throws when something goes wrong (bad item ID, negative gold,
//            file access failure).
// =====================================================================

#ifndef CUSTOM_EXCEPTIONS_H            // Header guard - prevents this file from
#define CUSTOM_EXCEPTIONS_H            // being included more than once per .cpp.

#include <exception>                   // std::exception is the standard base class
                                       // for all C++ exceptions; we inherit from it.
#include <string>                      // std::string used to store readable messages.

// ---------------------------------------------------------------------
// InvalidItemID
//   Thrown when the player types an item ID or rental record ID that
//   does not exist anywhere in the guild's records.
// ---------------------------------------------------------------------
class InvalidItemID : public std::exception {   // Public inheritance from std::exception
                                                // so try/catch(std::exception&) catches it too.
private:
    std::string message;               // Holds the formatted error text we'll return.

public:
    // 'explicit' stops the compiler from doing surprise conversions like
    // throwing a raw string where an InvalidItemID is expected.
    explicit InvalidItemID(const std::string& id)
        : message("Invalid item ID: '" + id +    // Build the user-facing message
                  "' was not found in the guild records.") {}

    // what() is the standard hook every exception subclass must override.
    // 'noexcept' tells the compiler it will never itself throw.
    // 'override' makes the compiler verify we're really overriding.
    const char* what() const noexcept override {
        return message.c_str();        // Return a C-string view of our std::string.
    }
};

// ---------------------------------------------------------------------
// NegativeGoldException
//   Thrown when an operation would push the guild's treasury below zero
//   (for example, a refund larger than current gold on hand).
// ---------------------------------------------------------------------
class NegativeGoldException : public std::exception {
private:
    std::string message;               // Same pattern as above - cached message.

public:
    explicit NegativeGoldException(const std::string& detail)
        : message("Gold cannot go negative: " + detail) {}   // Prefix + caller's detail.

    const char* what() const noexcept override {
        return message.c_str();
    }
};

// ---------------------------------------------------------------------
// FileAccessException
//   Thrown by FileHandler when the save file can't be opened, can't be
//   written, or contains malformed lines we can't parse.
// ---------------------------------------------------------------------
class FileAccessException : public std::exception {
private:
    std::string message;

public:
    explicit FileAccessException(const std::string& detail)
        : message("File access error: " + detail) {}

    const char* what() const noexcept override {
        return message.c_str();
    }
};

#endif // CUSTOM_EXCEPTIONS_H          // Closes the header guard opened at the top.
