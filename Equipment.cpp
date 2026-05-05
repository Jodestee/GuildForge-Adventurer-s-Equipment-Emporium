// =====================================================================
//  File:     Equipment.cpp
//  Project:  GuildForge: Adventurer's Equipment Emporium
//  Course:   CSCI 272 - Object-Oriented Programming in C++
//  Group:    Group 2 - "The Boys"
//  Owner:    Sharif, Yasin
//  Purpose:  Implementations for the Equipment base class. The default
//            displayDetails() and useItem() are written so that even
//            an "unspecialized" Equipment object would still print
//            something sensible - though in practice only the derived
//            class overrides ever run.
// =====================================================================

#include "Equipment.h"               // Our matching header.
#include "Visuals.h"                 // Color codes for pretty printing.
#include <iomanip>                   // std::fixed, std::setprecision for currency.

// ---- Default constructor ----
// Initialize every field to a safe "empty" default so an Equipment object
// is always in a valid state, even if someone forgets to populate it.
Equipment::Equipment()
    : id(""), name("Unknown"),
      basePrice(0.0), available(true),
      description("") {}

// ---- Full constructor ----
// Member-initializer list copies each parameter into the matching field.
// This is more efficient than assigning inside the body.
Equipment::Equipment(const std::string& id,
                     const std::string& name,
                     double basePrice,
                     bool available,
                     const std::string& description)
    : id(id), name(name),
      basePrice(basePrice), available(available),
      description(description) {}

// ---- displayDetails ----
// The base-class version. Each derived class overrides this with its
// own type-specific output (damage, defense, healing, etc.) but they
// can also call this to print the shared header lines if they want.
void Equipment::displayDetails() const {
    using namespace Visuals;                                  // Shortens Color:: usage.

    // Line 1: "[ID] Name (Equipment)"
    std::cout << "  " << Color::GOLD << "[" << id << "]"      // Gold ID tag.
              << Color::RESET
              << " " << Color::BOLD << name << Color::RESET   // Bold name.
              << "  " << Color::DIM << "(" << typeTag() << ")"// Dim type label.
              << Color::RESET << "\n";

    // Line 2: indented description in dim grey.
    std::cout << "      " << Color::DIM << description
              << Color::RESET << "\n";

    // Line 3: price + availability
    std::cout << "      Price: " << Color::GOLD << "$"
              << std::fixed << std::setprecision(2)           // Always 2 decimals.
              << basePrice << "/day" << Color::RESET
              << "   Status: ";

    if (available) {                                          // Color the status:
        std::cout << Color::SUCCESS << "Available"            //   green if available
                  << Color::RESET << "\n";
    } else {
        std::cout << Color::ERROR   << "Rented Out"           //   red if rented out
                  << Color::RESET << "\n";
    }
}

// ---- useItem ----
// Generic placeholder; weapons / armor / potions all override this.
std::string Equipment::useItem() {
    return name + " is used in a generic way.";
}

// ---- rent / returnItem ----
// Tiny helpers that just flip the availability flag.
void Equipment::rent()       { available = false; }
void Equipment::returnItem() { available = true;  }

// ---- operator== ----
// Two pieces of equipment count as "equal" when they share an ID.
// (We never have two items with the same ID in our shop.)
bool Equipment::operator==(const Equipment& other) const {
    return this->id == other.id;
}

// ---- operator<< ----
// Lets you write things like  std::cout << someEquipment;
// Friend function: defined outside the class but allowed to read
// the private members because the class declared it a friend.
std::ostream& operator<<(std::ostream& os, const Equipment& eq) {
    os << "[" << eq.id << "] " << eq.name
       << " (" << eq.typeTag() << ") - $"
       << std::fixed << std::setprecision(2) << eq.basePrice
       << "/day";
    return os;                       // Return os so calls can be chained.
}
