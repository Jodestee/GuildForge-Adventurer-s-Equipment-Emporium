// =====================================================================
//  File:     Armor.h
//  Project:  GuildForge: Adventurer's Equipment Emporium
//  Course:   CSCI 272 - Object-Oriented Programming in C++
//  Group:    Group 2 - "The Boys"
//  Owner:    Vittini, Kelvin
//  Purpose:  Derived class for protective gear (vests, mail, shields).
//            Adds defense + durability to the Equipment base.
// =====================================================================

#ifndef ARMOR_H
#define ARMOR_H

#include "Equipment.h"               // Inherit from Equipment.

class Armor : public Equipment {     // Public inheritance: IS-A Equipment.
private:
    int defense;    //how much armor is reduced                 // How much damage this armor absorbs.
    int durability;    // goes down when hit             // Decreases when hit; armor breaks at 0.

public:
    Armor();                                           // Default constructor.
    Armor(const std::string& id,                       // Full constructor.
          const std::string& name,
          double basePrice,
          bool available,
          const std::string& description,
          int defense,
          int durability);
//^^^^^^^^^^this is a full const setting all fields. the last two lines are how strong and how long it can last.^^^^^^^
    // ---- Polymorphic overrides ----
    void displayDetails() const override;// shows all armor info on screen
    std::string useItem() override;//what happens when the armor is used
    std::string typeTag() const override { return "Armor"; }//returns the item type as text

    // Accessors.
    int getDefense()    const { return defense; }//read armor's defense
    int getDurability() const { return durability; }//reads how much durability
};

#endif // ARMOR_H
