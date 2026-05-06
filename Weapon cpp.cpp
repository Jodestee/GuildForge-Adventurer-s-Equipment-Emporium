// =====================================================================
//  File:     Weapon.cpp
//  Project:  GuildForge: Adventurer's Equipment Emporium
//  Course:   CSCI 272 - Object-Oriented Programming in C++
//  Group:    Group 2 - "The Boys"
//  Owner:    Vittini, Kelvin
//  Purpose:  Implements the Weapon class - a kind of Equipment that
//            simulates combat damage and loses durability with each use.
// =====================================================================

#include "Weapon.h"
#include "Visuals.h"                 // Color codes for stylized output.
#include <iomanip>                   // For std::fixed and std::setprecision.
#include <sstream>                   // std::ostringstream lets us build
                                     // multi-piece strings cleanly.
#include <cstdlib>                   // std::rand() for damage variance.

// ---- Default constructor ----
// Calls Equipment() to initialize base fields, then zeros our new fields.
Weapon::Weapon()  
    : Equipment(), damage(0), durability(0) {}
// calls the base class, sets this weapon damage to 0, and sets durability to 0.
// ---- Full constructor ----
// Forwards the base fields to Equipment(...) and stores damage/durability.
Weapon::Weapon(const std::string& id, //the weapons identifier
               const std::string& name,// what the weapon is called
               double basePrice,// how much it costs
               bool available,//whether its in stock
               const std::string& description,//words describing the weapon
               int damage,// how powerfull it is
               int durability) // how long can the weapon be used
    : Equipment(id, name, basePrice, available, description), //this gives the base class all the info it needs
      damage(damage), durability(durability) {} //

// ---- displayDetails (override) ----
// Same general layout as the base version but adds Damage and Durability,
// and the ID tag is colored red to visually flag "this is a weapon".
void Weapon::displayDetails() const {
    using namespace Visuals;

    // Header line: red ID, bold name, dim type tag.
    std::cout << "  " << Color::B_RED << "[" << id << "]" << Color::RESET
              << " " << Color::BOLD  << name << Color::RESET
              << "  " << Color::DIM << "(Weapon)" << Color::RESET << "\n";

    // Description in dim grey.
    std::cout << "      " << Color::DIM << description
              << Color::RESET << "\n";

    // Stats line: price (gold), damage (red), durability (yellow), status.
    std::cout << "      Price: " << Color::GOLD << "$"
              << std::fixed << std::setprecision(2) << basePrice
              << "/day" << Color::RESET
              << "   Damage: "     << Color::B_RED    << damage
              << Color::RESET
              << "   Durability: " << Color::B_YELLOW << durability
              << Color::RESET
              << "   Status: ";
//^^^^^^^^^all this up here is talkling about formatted weapons, ID displays and so on. 
// using different colors and spacing to make everything look nice
    if (available) {
        std::cout << Color::SUCCESS << "Available"   << Color::RESET << "\n";
    } else {
        std::cout << Color::ERROR   << "Rented Out" << Color::RESET << "\n";
    }
}
//^^^^^^^^^^This code checks whether the item is available; 
//if it is, it prints “Available” in green, 
//and if not, 
//it prints “Rented Out” in red.
// ---- useItem (override) ----
// Simulates one combat strike: rolls damage with small variance,
// decrements durability, and breaks the weapon if durability hits 0.
std::string Weapon::useItem() {
    std::ostringstream out;                                   // Build result string.

    // Roll: damage +/- a small random variance, clamped to at least 1.
    int rolledDamage = damage + (std::rand() % 10) - 4;//this makes the attack damge feel random
    if (rolledDamage < 1) rolledDamage = 1;//makes sure dmg never drops below 1

    out << ">> " << name << " strikes for "    //describes the attack              // Combat flavor line.
        << rolledDamage << " damage! ";

    if (durability > 0) {       //see if weapon is still durable                             // Still has durability...
        durability--;          //reduce durability by 1                              // ...so use one charge.
        if (durability == 0) {                               // Did it break?
            available = false;                               // Yes - mark unavailable.
            out << "The blade shatters! (item broken)";
        } else {
            out << "(durability remaining: " << durability << ")";
        }
    } else {                                                 // Already broken.
        out << "(weapon already broken)";
    }
    return out.str();                                        // Hand back the message.
}
