// =====================================================================
//  File:     Armor.cpp
//  Project:  GuildForge: Adventurer's Equipment Emporium
//  Course:   CSCI 272 - Object-Oriented Programming in C++
//  Group:    Group 2 - "The Boys"
//  Owner:    Vittini, Kelvin
//  Purpose:  Implements Armor - a kind of Equipment that absorbs damage
//            during random events and loses durability with use.
// =====================================================================

#include "Armor.h"
#include "Visuals.h"                 // Color helpers.
#include <iomanip>                   // For std::fixed / std::setprecision.
#include <sstream>                   // std::ostringstream for building strings.
#include <cstdlib>                   // std::rand() for absorption variance.

// ---- Default constructor ----
Armor::Armor()
    : Equipment(), defense(0), durability(0) {}

// ---- Full constructor ----
// Forwards base-class fields up, stores defense and durability locally.
Armor::Armor(const std::string& id,//the armors unique ID
             const std::string& name,//the armors name
             double basePrice,//how much the armor cost
             bool available,//wether it can be rented or used
             const std::string& description,//words describing the armor
             int defense,//the block amount
             int durability)//how long untill the item breaks apart
    : Equipment(id, name, basePrice, available, description),//gives values to the base class
      defense(defense), durability(durability) {}

// ---- displayDetails (override) ----
// Color-codes the ID in blue so armor visually pops as "defensive gear".
void Armor::displayDetails() const {
    using namespace Visuals;

    // Header line.
    std::cout << "  " << Color::B_BLUE << "[" << id << "]" << Color::RESET
              << " " << Color::BOLD   << name << Color::RESET
              << "  " << Color::DIM << "(Armor)" << Color::RESET << "\n";

    // Description.
    std::cout << "      " << Color::DIM << description
              << Color::RESET << "\n";

    // Stats line: price (gold), defense (blue), durability (yellow), status.
    std::cout << "      Price: " << Color::GOLD << "$"
              << std::fixed << std::setprecision(2) << basePrice
              << "/day" << Color::RESET
              << "   Defense: "    << Color::B_BLUE   << defense
              << Color::RESET
              << "   Durability: " << Color::B_YELLOW << durability
              << Color::RESET
              << "   Status: ";

    if (available) {
        std::cout << Color::SUCCESS << "Available"   << Color::RESET << "\n";
    } else {
        std::cout << Color::ERROR   << "Rented Out" << Color::RESET << "\n";
    }
}
//^^^^^^^^^^^^The code prints the armor’s ID in blue, its name in bold, and the label “(Armor)” in dim text. 
//Then it prints the armor’s description in dim gray underneath. 
//Finally, it shows a stats line with the price in gold, the defense in blue, the durability in yellow, and leaves space to print the status afterward.
//^^^^^^^^^^^^making the display look nice and colorful^^^^^^^^^^^^^^^
// ---- useItem (override) ----
// Armor absorbs a chunk of incoming damage and chips its durability.
std::string Armor::useItem() {
    std::ostringstream out;                       // Buffer for our message.

    // Absorption = base defense + small random bonus 0..4.
    int reduction = defense + (std::rand() % 5);

    out << ">> " << name << " absorbs " << reduction
        << " damage, shielding the adventurer! ";

    if (durability > 0) {                         // Still has uses left?
        durability--;                             // Wear it down by one.
        if (durability == 0) {                    // Broke on this hit?
            available = false;                    // Permanently unavailable.
            out << "The armor cracks beyond repair! (item broken)";
        } else {
            out << "(durability remaining: " << durability << ")";
        }
    } else {
        out << "(armor already broken)";
    }
    return out.str();
}
