// =====================================================================
//  File:     GuildManager.h
//  Project:  GuildForge: Adventurer's Equipment Emporium
//  Course:   CSCI 272 - Object-Oriented Programming in C++
//  Group:    Group 2 - "The Boys"
//  Owner:    Jn Francois, Jodestee
//  Purpose:  The controller class. Owns the inventory and rentals,
//            tracks gold/reputation/day, and runs the main game loop.
//            All business logic and menu handling lives here, leaving
//            main() trivial (just constructs and calls run()).
// =====================================================================

#ifndef GUILD_MANAGER_H
#define GUILD_MANAGER_H

#include <vector>
#include <string>
#include "Equipment.h"
#include "RentalRecord.h"

class GuildManager {
private:
    // The two STL containers required by the rubric. The inventory holds
    // base-class pointers so polymorphism works through it; the manager
    // owns these pointers and deletes them in the destructor.
    std::vector<Equipment*>   inventory;
    std::vector<RentalRecord> activeRentals;

    double gold;                     // Current treasury.
    int    reputation;               // Reputation score (-/+).
    int    currentDay;               // Current in-game day counter.
    int    nextRecordNumber;         // Counter used to mint R001, R002, R003...
    const  std::string saveFile;     // Path to guild_save.csv.

    // ---- Internal menu handlers ----
    // None of these are public; only run() is.
    void displayMainMenu() const;
    void viewInventory() const;
    void rentEquipment();
    void returnItem();
    void handleRandomEvent();
    void viewActiveRentals() const;
    void viewDailyReport() const;
    void saveAndQuit(bool& running);

    // ---- First-run / loading helpers ----
    void createNewGuild();           // Stocks an empty starter inventory.
    void loadFromDisk();              // Tries to load saved state.

    // ---- Small utilities ----
    std::string nextRecordId();                     // "R001" -> "R002" -> ...
    Equipment* findItemById(const std::string& id); // Wrapper around template search.
    RentalRecord* findRentalById(const std::string& id);
    std::vector<std::string> sampleCustomerNames() const;

public:
    // Constructor takes the save-file path (defaults to guild_save.csv).
    GuildManager(const std::string& saveFile = "guild_save.csv");
    ~GuildManager();                 // Frees the owned Equipment* pointers.

    // We own raw pointers, so prevent accidental copying that would
    // double-delete on destruction. '= delete' tells the compiler
    // "do not generate these functions".
    GuildManager(const GuildManager&)            = delete;
    GuildManager& operator=(const GuildManager&) = delete;

    // The one public entry point. main() just calls this.
    void run();
};

#endif // GUILD_MANAGER_H
