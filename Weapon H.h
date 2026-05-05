// =====================================================================
//  File:     Weapon.h
//  Project:  GuildForge: Adventurer's Equipment Emporium
//  Course:   CSCI 272 - Object-Oriented Programming in C++
//  Group:    Group 2 - "The Boys"
//  Owner:    Vittini, Kelvin
//  Purpose:  Derived class representing weapons (swords, bows, axes).
//            Adds 'damage' and 'durability' on top of Equipment, and
//            overrides displayDetails() and useItem() so the player
//            sees combat-style output specific to weapons.
// =====================================================================

#ifndef WEAPON_H
#define WEAPON_H

#include "Equipment.h"               // We inherit from Equipment.

// 'public Equipment' = Weapon IS-A Equipment, with public access preserved.
// this section has to do with: 
// weapon type
// how much harm can be dealt with weapon
// how long can it be used before it breaks
class Weapon : public Equipment {
private:                             // Weapon-specific fields, hidden from outside.
    int damage;                      // Combat damage value the weapon deals.
    int durability;                  // How many uses before the weapon breaks.
//
public:
    Weapon();  //this creats a weapon object                                        // Default constructor.
    Weapon(const std::string& id,  //the weapons ID                    // Full constructor takes
           const std::string& name,  //the weapons name                  // every base-class field
           double basePrice,   //how much the weapon costs                        // PLUS the two new ones.
           bool available,     //checks wether the weapon is in stock
           const std::string& description,   //describes the weapon
           int damage,  //the amount of damage
           int durability);  //how long the weapon last before it breaks

    // ---- Polymorphic overrides ----
    // 'override' tells the compiler "I am replacing a virtual function";
    // if the signature doesn't match the base class, compilation fails -
    // a great safety net.
    void displayDetails() const override;  //this will show the weapons information
    std::string useItem() override;   //this function is called when the weapon is used
    std::string typeTag() const override { return "Weapon"; }
//^^^^^^ returns a lable that identifies what kind of item it is^^^^
    // Read-only accessors for the new fields.
    int getDamage()     const { return damage; }  //this reads the weapons damage value
    int getDurability() const { return durability; } //cheacks the durability that is left
};

#endif // WEAPON_H
