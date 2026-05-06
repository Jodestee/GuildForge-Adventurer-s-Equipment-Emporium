// =====================================================================
//  File:     Potion.h
//  Project:  GuildForge: Adventurer's Equipment Emporium
//  Course:   CSCI 272 - Object-Oriented Programming in C++
//  Group:    Group 2 - "The Boys"
//  Owner:    Vittini, Kelvin
//  Purpose:  Derived class for consumable potions. Restores HP when
//            used; single-use potions become unavailable after use.
// =====================================================================

#ifndef POTION_H
#define POTION_H

#include "Equipment.h"               // Inherit from Equipment.

class Potion : public Equipment {  //
private:
    int  healingAmount;     //but how many points?         // Hit points restored on use.
    bool consumedOnUse;     //dissapears after use         // True for single-use potions.

public:
    Potion();                       //creates a empty basic potion  // Default constructor.
    Potion(const std::string& id,                      // Full constructor.
           const std::string& name,
           double basePrice,
           bool available,
           const std::string& description,
           int healingAmount,//how much health the potion gives
           bool consumedOnUse);//single use potion

    // ---- Polymorphic overrides ----
    void displayDetails() const override;//shows all potion info on screen
    std::string useItem() override;//single use
    std::string typeTag() const override { return "Potion"; }//returns item type as text

    // Accessors.
    int  getHealingAmount() const { return healingAmount; }//reads how much it heals
    bool isConsumedOnUse()  const { return consumedOnUse; }//single us
};

#endif // POTION_H
