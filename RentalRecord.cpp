// =====================================================================
//  File:     RentalRecord.cpp
//  Project:  GuildForge: Adventurer's Equipment Emporium
//  Course:   CSCI 272 - Object-Oriented Programming in C++
//  Group:    Group 2 - "The Boys"
//  Owner:    Sharif, Yasin
//  Purpose:  Implements RentalRecord, including the operator+ overload
//            (rubric-required) and the fee/overdue calculation logic.
// =====================================================================

#include "RentalRecord.h"
#include "Visuals.h"                 // Color helpers.
#include <iostream>
#include <iomanip>                   // std::fixed / std::setprecision for $ output.

// ---- Default constructor ----
// Empty record - useful as the starting accumulator for operator+ chains.
RentalRecord::RentalRecord()
    : recordId(""), customerName("Unknown"),
      rentedItem(nullptr),           // Null pointer = unlinked / no item yet.
      rentalDay(0), rentalDuration(0),
      totalFee(0.0) {}

// ---- Full constructor ----
RentalRecord::RentalRecord(const std::string& recordId,
                           const std::string& customerName,
                           Equipment* rentedItem,
                           int rentalDay,
                           int rentalDuration)
    : recordId(recordId), customerName(customerName),
      rentedItem(rentedItem),
      rentalDay(rentalDay), rentalDuration(rentalDuration),
      totalFee(0.0) {}

// ---- calculateFee ----
// Computes what this customer owes based on 'currentDay'. The base fee
// is (item price) x (rentalDuration). If overdue, 50% of base price is
// added per day late.
double RentalRecord::calculateFee(int currentDay) const {
    if (!rentedItem) return 0.0;                         // No item linked => no fee.

    int daysUsed = currentDay - rentalDay;               // How long they've held it.
    if (daysUsed < 1) daysUsed = 1;                      // Minimum one day's charge.

    double base = rentedItem->getBasePrice() * rentalDuration;  // Agreed fee.

    if (isOverdue(currentDay)) {                         // If they're late...
        int overdueDays = daysUsed - rentalDuration;
        double surcharge = rentedItem->getBasePrice() * 0.5 * overdueDays; // ...50%/day extra.
        return base + surcharge;
    }
    return base;
}

// ---- isOverdue ----
// True if the customer has had the item longer than agreed.
bool RentalRecord::isOverdue(int currentDay) const {
    return (currentDay - rentalDay) > rentalDuration;
}

// ---- operator+ ----
// Adds two records together by summing their totalFee values. Returns a
// summary RentalRecord with recordId="TOTAL". Used like:
//     RentalRecord running;            // starts empty (totalFee = 0)
//     for (auto& r : rentals)
//         running = running + r;       // sum every rental into 'running'
RentalRecord RentalRecord::operator+(const RentalRecord& other) const {
    RentalRecord combined;                        // Start with an empty record.
    combined.recordId       = "TOTAL";            // Special tag so display can spot it.
    combined.customerName   = "(merged daily revenue)";
    combined.rentedItem     = nullptr;            // No single item to point at.
    combined.rentalDay      = this->rentalDay;
    combined.rentalDuration = this->rentalDuration + other.rentalDuration;
    combined.totalFee       = this->totalFee + other.totalFee;   // The actual sum.
    return combined;
}

// ---- displayRecord ----
// Pretty-prints this rental in two indented lines.
void RentalRecord::displayRecord() const {
    using namespace Visuals;

    // Line 1: record ID + customer.
    std::cout << "  " << Color::B_MAGENTA << "[" << recordId << "]"
              << Color::RESET
              << "  Customer: " << Color::BOLD << customerName
              << Color::RESET << "\n";

    // Line 2: the linked item (or "unlinked" if pointer is null).
    if (rentedItem) {
        std::cout << "      Item:     [" << rentedItem->getId() << "] "
                  << rentedItem->getName() << "\n";
    } else {
        std::cout << "      Item:     " << Color::DIM
                  << "(unlinked)" << Color::RESET << "\n";
    }

    // Line 3: timing + fee (only show fee if it's been computed).
    std::cout << "      Rented day " << rentalDay
              << " for " << rentalDuration << " day(s)";
    if (totalFee > 0.0) {
        std::cout << "   Fee: " << Color::GOLD << "$"
                  << std::fixed << std::setprecision(2) << totalFee
                  << Color::RESET;
    }
    std::cout << "\n";
}
