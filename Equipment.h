// =====================================================================
//  File:     Equipment.h
//  Project:  GuildForge: Adventurer's Equipment Emporium
//  Course:   CSCI 272 - Object-Oriented Programming in C++
//  Group:    Group 2 - "The Boys"
//  Owner:    Sharif, Yasin
//  Purpose:  Defines the Equipment base class - the parent of every
//            rentable item (Weapon, Armor, Potion). Holds the shared
//            data members (id, name, price, availability, description)
//            and declares the polymorphic interface used everywhere
//            else in the project.
// =====================================================================

#ifndef EQUIPMENT_H                  // Header guard: include this file at most
#define EQUIPMENT_H                  // once per translation unit.

#include <string>                    // std::string for textual data members.
#include <iostream>                  // std::ostream for the operator<< friend.

class Equipment {
protected:                           // 'protected' = derived classes can read these,
                                     // but other code outside the family cannot.
    std::string id;                  // Unique identifier (e.g. "W001", "A002").
    std::string name;                // Display equipments name shown to the player.
    double      basePrice;           // Daily rental price in gold.
    bool        available;           // True if not currently rented out.
    std::string description;         // Flavor text shown in inventory listings.

public:
    // ---- Constructors / destructor ----
    Equipment();                     // Default constructor (used by derived defaults).
    Equipment(const std::string& id,             // Full constructor - all fields.
              const std::string& name,
              double basePrice,
              bool available,
              const std::string& description);

    // 'virtual' destructor is critical here because we delete derived
    // objects through Equipment* pointers. Without it, ~Weapon() etc.
    // would never run (object slicing on destruction).
    virtual ~Equipment() = default;

    // -----------------------------------------------------------------
    // Polymorphic interface - the heart of the inheritance design.
    // Each derived class overrides these. Calling code uses Equipment*
    // and the right version runs at runtime thanks to virtual dispatch.
    // -----------------------------------------------------------------
    virtual void displayDetails() const;           // Pretty-print this item.
    virtual std::string useItem();                 // Simulate "using" the item.

    // typeTag is what gets written to the CSV ("Weapon"/"Armor"/"Potion").
    // Default returns "Equipment"; each derived class overrides it.
    virtual std::string typeTag() const { return "Equipment"; }

    // ---- Non-virtual helpers shared by every item ----
    void rent();                     // Mark item as rented out.
    void returnItem();               // Mark item as available again.

    // ---- Read-only accessors ('const' = doesn't modify the object) ----
    const std::string& getId() const          { return id; }
    const std::string& getName() const        { return name; }
    double             getBasePrice() const   { return basePrice; }
    bool               isAvailable() const    { return available; }
    const std::string& getDescription() const { return description; }

    // -----------------------------------------------------------------
    // Operator overloads (one of the rubric's required features).
    //   ==  : compare two items by ID.
    //   <<  : print an item to any std::ostream (cout, file, etc).
    //         Declared as 'friend' so it can access private members,
    //         even though it isn't a member function itself.
    // -----------------------------------------------------------------
    bool operator==(const Equipment& other) const;
    friend std::ostream& operator<<(std::ostream& os, const Equipment& eq);
};

#endif // EQUIPMENT_H
