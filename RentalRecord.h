// =====================================================================
//  File:     RentalRecord.h
//  Project:  GuildForge: Adventurer's Equipment Emporium
//  Course:   CSCI 272 - Object-Oriented Programming in C++
//  Group:    Group 2 - "The Boys"
//  Owner:    Sharif, Yasin
//  Purpose:  Represents one rental transaction: which customer, which
//            item (Equipment* base-class pointer), the day it started,
//            agreed duration, and the computed fee. Also carries the
//            operator+ overload used to merge daily rental totals.
// =====================================================================

#ifndef RENTAL_RECORD_H
#define RENTAL_RECORD_H

#include <string>
#include "Equipment.h"               // We hold an Equipment* pointer.

class RentalRecord {
private:
    std::string recordId;            // Unique rental ID, e.g. "R001", "R002".
    std::string customerName;        // Name of the NPC adventurer renting.
    Equipment*  rentedItem;          // Base-class pointer; we DO NOT own it
                                     // (GuildManager owns and deletes items).
    int    rentalDay;                // Game-day on which the rental began.
    int    rentalDuration;           // How many days were agreed up front.
    double totalFee;                 // Accumulated fee owed for this rental.

public:
    RentalRecord();                                    // Default constructor.
    RentalRecord(const std::string& recordId,          // Full constructor.
                 const std::string& customerName,
                 Equipment* rentedItem,
                 int rentalDay,
                 int rentalDuration);

    // ---- Fee logic ----
    // Returns the fee owed if the customer returns on 'currentDay'.
    // Adds a 50%-per-day surcharge for any days past the agreed duration.
    double calculateFee(int currentDay) const;

    // True if the rental is past its agreed duration.
    bool isOverdue(int currentDay) const;

    // -----------------------------------------------------------------
    // Operator+ overload (rubric requirement).
    //   Treats two RentalRecords as items in a running daily revenue
    //   tally - the resulting record's totalFee is the sum of the two.
    //   Used in "Daily Report" to combine all active rentals.
    // -----------------------------------------------------------------
    RentalRecord operator+(const RentalRecord& other) const;

    // Pretty-print this record using Visuals colors.
    void displayRecord() const;

    // ---- Accessors ----
    const std::string& getRecordId()      const { return recordId; }
    const std::string& getCustomerName()  const { return customerName; }
    Equipment*         getRentedItem()    const { return rentedItem; }
    int                getRentalDay()     const { return rentalDay; }
    int                getRentalDuration()const { return rentalDuration; }
    double             getTotalFee()      const { return totalFee; }

    // ---- Mutators ----
    void setTotalFee(double fee)        { totalFee = fee; }
    // Used after loading from disk to re-link the pointer to the matching
    // Equipment object once the inventory has been rebuilt.
    void setRentedItem(Equipment* item) { rentedItem = item; }
};

#endif // RENTAL_RECORD_H
