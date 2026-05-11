// =====================================================================
//  File:     FileHandler.h
//  Project:  GuildForge: Adventurer's Equipment Emporium
//  Course:   CSCI 272 - Object-Oriented Programming in C++
//  Group:    Group 2 - "The Boys"
//  Owner:    Jn Francois, Jodestee
//  Purpose:  Encapsulates all CSV save/load operations for the game.
//            Uses std::stringstream to parse and format records, which
//            satisfies the rubric's "meaningful stringstream usage"
//            requirement, and produces/consumes a single guild_save.csv.
// =====================================================================

#ifndef FILE_HANDLER_H
#define FILE_HANDLER_H

#include <string>
#include <vector>
#include "Equipment.h"               // Inventory holds Equipment* pointers.
#include "RentalRecord.h"            // Rentals are RentalRecord values.

// ---------------------------------------------------------------------
// GuildState
//   A simple snapshot bundle returned by loadFromFile(). Plain struct -
//   no methods, just a record of "everything we read off disk".
// ---------------------------------------------------------------------
struct GuildState {
    std::vector<Equipment*> inventory;       // All items, freshly allocated.
    std::vector<RentalRecord> rentals;       // All active rentals.
    double gold       = 0.0;                 // Current treasury.
    int    reputation = 0;                   // Current reputation score.
    int    currentDay = 1;                   // Current in-game day.
    bool   fileExisted = false;              // false on first run (no save file).
};

class FileHandler {
public:
    // -----------------------------------------------------------------
    // loadFromFile
    //   Reads 'filename' and returns a GuildState. If the file doesn't
    //   exist (first run) returns an empty state with fileExisted=false.
    //   Throws FileAccessException for any malformed/unreadable case.
    // -----------------------------------------------------------------
    static GuildState loadFromFile(const std::string& filename);

    // -----------------------------------------------------------------
    // saveToFile
    //   Writes the entire guild state to 'filename' in our CSV format.
    //   Throws FileAccessException on write failure.
    // -----------------------------------------------------------------
    static void saveToFile(const std::string& filename,
                           const std::vector<Equipment*>& inventory,
                           const std::vector<RentalRecord>& rentals,
                           double gold,
                           int reputation,
                           int currentDay);
};

#endif // FILE_HANDLER_H
