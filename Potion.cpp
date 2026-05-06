// =====================================================================
//  File:     Potion.cpp
//  Project:  GuildForge: Adventurer's Equipment Emporium
//  Course:   CSCI 272 - Object-Oriented Programming in C++
//  Group:    Group 2 - "The Boys"
//  Owner:    Vittini, Kelvin
//  Purpose:  Implements Potion - a one-shot Equipment that heals an
//            adventurer and (usually) marks itself unavailable after.
// =====================================================================

#include "Potion.h"
#include "Visuals.h"                 // For colored output.
#include <iomanip>                   // For std::fixed / std::setprecision.
#include <sstream>                   // std::ostringstream for building strings.

// ---- Default constructor ----
Potion::Potion()
    : Equipment(), healingAmount(0), consumedOnUse(true) {}

// ---- Full constructor ----
// Forwards the base fields and stores the potion-specific ones.
Potion::Potion(const std::string& id,
               const std::string& name,
               double basePrice,
               bool available,
               const std::string& description,
               int healingAmount,
               bool consumedOnUse)
    : Equipment(id, name, basePrice, available, description),
      healingAmount(healingAmount), consumedOnUse(consumedOnUse) {}
//^^^^^^^^^^AGAIN WITH THE CONST^^^^^^^^^^^^^
// ---- displayDetails (override) ----
// Green ID tag + green healing number to signal "this restores life".
void Potion::displayDetails() const {
    using namespace Visuals;
//^^^^^^^^^HOW WE MAKE IT PRETTY FOR THE DISPLAYvvvvvvvvvvvvv
    // Header line.
    std::cout << "  " << Color::B_GREEN << "[" << id << "]" << Color::RESET
              << " " << Color::BOLD    << name << Color::RESET
              << "  " << Color::DIM << "(Potion)" << Color::RESET << "\n";

    // Description.
    std::cout << "      " << Color::DIM << description
              << Color::RESET << "\n";

    // Stats line: price, healing amount, single-use flag, status.
    std::cout << "      Price: " << Color::GOLD << "$"
              << std::fixed << std::setprecision(2) << basePrice
              << "/day" << Color::RESET
              << "   Heals: " << Color::B_GREEN << "+" << healingAmount
              << " HP" << Color::RESET
              << "   Single-use: " << (consumedOnUse ? "yes" : "no")
              << "   Status: ";

    if (available) {
        std::cout << Color::SUCCESS << "Available"   << Color::RESET << "\n";
    } else {
        std::cout << Color::ERROR   << "Rented Out" << Color::RESET << "\n";
    }
}

// ---- useItem (override) ----
// Heals the adventurer; if the potion is single-use, it self-destructs.
std::string Potion::useItem() {
    std::ostringstream out;
    out << ">> " << name << " restores " << healingAmount
        << " HP! The adventurer feels revitalized.";
    if (consumedOnUse) {                         // One-shot potion?
        available = false;                       // Mark it as gone.
        out << " (potion consumed)";
    }
    return out.str();
}
