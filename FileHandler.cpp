// =====================================================================
//  File:     FileHandler.cpp
//  Project:  GuildForge: Adventurer's Equipment Emporium
//  Course:   CSCI 272 - Object-Oriented Programming in C++
//  Group:    Group 2 - "The Boys"
//  Owner:    Jn Francois, Jodestee
//  Purpose:  Implements CSV save/load. This is where the rubric's
//            stringstream + file I/O + dynamic polymorphic construction
//            requirements all converge.
//
//  CSV format (one record per line, tagged so we know what it is):
//
//      META,gold,reputation,currentDay
//      ITEM,id,name,type,price,available,description,stat1,stat2
//      RENTAL,recordId,customer,itemId,rentalDay,duration,totalFee
//
//  Type-specific stat columns:
//      Weapon -> damage, durability
//      Armor  -> defense, durability
//      Potion -> healingAmount, consumedOnUse(0/1)
//
//  Embedded commas in names/descriptions are escaped as "\;" so the
//  splitter never trips on them.
// =====================================================================

#include "FileHandler.h"
#include "Weapon.h"
#include "Armor.h"
#include "Potion.h"
#include "CustomExceptions.h"        // FileAccessException lives here.
#include "Utility.h"                 // searchPointerByAttribute<> for re-linking.

#include <fstream>                   // std::ifstream / std::ofstream for files.
#include <sstream>                   // std::stringstream for parsing/building lines.
#include <iostream>

// ---------------------------------------------------------------------
// Anonymous namespace = file-private helpers, hidden from other .cpp's.
// ---------------------------------------------------------------------
namespace {

    // Splits a CSV line on commas using a stringstream. Simple because
    // we escape literal commas inside fields before saving.
    std::vector<std::string> splitCSV(const std::string& line) {
        std::vector<std::string> tokens;       // Output.
        std::stringstream ss(line);            // Wrap line in a stringstream.
        std::string token;
        while (std::getline(ss, token, ',')) { // getline with ',' as delimiter
            tokens.push_back(token);           // collects each comma-separated chunk.
        }
        return tokens;
    }

    // Replace literal commas in a field with "\;" so they survive the
    // CSV round-trip. Also strips embedded newlines.
    std::string escapeField(const std::string& s) {
        std::string out;
        out.reserve(s.size());                 // Pre-allocate to avoid resizes.
        for (char c : s) {
            if (c == ',')                      out += "\\;";   // comma -> \;
            else if (c == '\n' || c == '\r')   out += ' ';     // newline -> space
            else                               out += c;       // else: keep.
        }
        return out;
    }

    // Inverse of escapeField. Run on each field after splitting.
    std::string unescapeField(const std::string& s) {
        std::string out;
        out.reserve(s.size());
        for (size_t i = 0; i < s.size(); ++i) {
            // If we see "\;" turn it back into ",".
            if (i + 1 < s.size() && s[i] == '\\' && s[i+1] == ';') {
                out += ',';
                ++i;                           // Skip the ';' we just consumed.
            } else {
                out += s[i];
            }
        }
        return out;
    }

}  // anonymous namespace

// ---------------------------------------------------------------------
// loadFromFile
// ---------------------------------------------------------------------
GuildState FileHandler::loadFromFile(const std::string& filename) {
    GuildState state;                          // Empty state we'll fill in.

    // Quick existence check that doesn't throw - lets us treat "no save
    // file yet" as a normal first-run case rather than an error.
    {
        std::ifstream check(filename);         // Try to open for reading.
        if (!check.is_open()) {                // Couldn't open => assume no file.
            state.fileExisted = false;
            return state;                      // Return empty state.
        }
    }

    // Real open for parsing.
    std::ifstream file(filename);
    if (!file.is_open()) {
        // File exists but we still can't open it (permissions, locked, ...).
        throw FileAccessException("Could not open '" + filename + "' for reading.");
    }
    state.fileExisted = true;                  // We will be loading real data.

    std::string line;
    while (std::getline(file, line)) {         // Read line by line.
        if (line.empty()) continue;            // Skip blanks.

        // Use a stringstream just to grab the line's tag (META/ITEM/RENTAL).
        std::stringstream lineStream(line);
        std::string tag;
        std::getline(lineStream, tag, ',');

        // ---------------- META ----------------
        if (tag == "META") {
            // Format: META,gold,reputation,currentDay
            std::string goldStr, repStr, dayStr;
            std::getline(lineStream, goldStr, ',');
            std::getline(lineStream, repStr,  ',');
            std::getline(lineStream, dayStr,  ',');
            try {
                // std::stod / std::stoi parse text into numbers; they
                // throw if the text isn't numeric.
                state.gold       = goldStr.empty() ? 0.0 : std::stod(goldStr);
                state.reputation = repStr.empty()  ? 0   : std::stoi(repStr);
                state.currentDay = dayStr.empty()  ? 1   : std::stoi(dayStr);
            } catch (const std::exception&) {
                throw FileAccessException("Malformed META line in save file.");
            }
        }
        // ---------------- ITEM ----------------
        else if (tag == "ITEM") {
            std::vector<std::string> tokens = splitCSV(line);
            if (tokens.size() < 9) {           // Sanity check column count.
                throw FileAccessException("Malformed ITEM line: '" + line + "'");
            }

            // Pull the columns out, unescaping where needed.
            const std::string& id          = tokens[1];
            const std::string  name        = unescapeField(tokens[2]);
            const std::string& type        = tokens[3];
            double price                   = std::stod(tokens[4]);
            bool available                 = (tokens[5] == "1");
            const std::string  description = unescapeField(tokens[6]);

            // Build the right derived class based on the type tag.
            // This is the "polymorphic construction from disk" step the
            // rubric specifically asks for.
            Equipment* item = nullptr;
            try {
                if (type == "Weapon") {
                    int damage     = std::stoi(tokens[7]);
                    int durability = std::stoi(tokens[8]);
                    item = new Weapon(id, name, price, available,
                                      description, damage, durability);
                }
                else if (type == "Armor") {
                    int defense    = std::stoi(tokens[7]);
                    int durability = std::stoi(tokens[8]);
                    item = new Armor(id, name, price, available,
                                     description, defense, durability);
                }
                else if (type == "Potion") {
                    int healing   = std::stoi(tokens[7]);
                    bool consumed = (tokens[8] == "1");
                    item = new Potion(id, name, price, available,
                                      description, healing, consumed);
                }
                else {
                    throw FileAccessException("Unknown item type '" + type + "'");
                }
            } catch (const std::exception& e) {
                // stoi/stod can throw - convert to our domain exception.
                throw FileAccessException(
                    std::string("Malformed ITEM stats: ") + e.what());
            }
            state.inventory.push_back(item);   // Add to the loaded inventory.
        }
        // ---------------- RENTAL ----------------
        else if (tag == "RENTAL") {
            std::vector<std::string> tokens = splitCSV(line);
            if (tokens.size() < 7) {
                throw FileAccessException("Malformed RENTAL line: '" + line + "'");
            }
            const std::string& recordId     = tokens[1];
            const std::string  customerName = unescapeField(tokens[2]);
            const std::string& itemId       = tokens[3];
            int    rentalDay      = std::stoi(tokens[4]);
            int    rentalDuration = std::stoi(tokens[5]);
            double totalFee       = std::stod(tokens[6]);

            // Re-link the Equipment* pointer to the just-loaded inventory.
            // Uses the template from Utility.h.
            Equipment* linked = searchPointerByAttribute<Equipment, std::string>(
                state.inventory,
                itemId,
                [](Equipment* e){ return e->getId(); });

            // It's OK if linked is nullptr - the record will just show
            // "(unlinked)" in the UI, but loading shouldn't fail.

            RentalRecord record(recordId, customerName, linked,
                                rentalDay, rentalDuration);
            record.setTotalFee(totalFee);
            state.rentals.push_back(record);
        }
        // Unknown tags are silently skipped for forward compatibility.
    }

    file.close();                              // Done reading.
    return state;
}

// ---------------------------------------------------------------------
// saveToFile
// ---------------------------------------------------------------------
void FileHandler::saveToFile(const std::string& filename,
                             const std::vector<Equipment*>& inventory,
                             const std::vector<RentalRecord>& rentals,
                             double gold,
                             int reputation,
                             int currentDay) {
    std::ofstream file(filename);              // Open for writing (truncates).
    if (!file.is_open()) {
        throw FileAccessException("Could not open '" + filename + "' for writing.");
    }

    // ---- META line ----
    {
        std::stringstream ss;                  // Build the line in a stream...
        ss << "META," << gold << "," << reputation << "," << currentDay;
        file << ss.str() << "\n";              // ...then write it as one piece.
    }

    // ---- ITEM lines (one per inventory entry) ----
    for (Equipment* item : inventory) {
        if (!item) continue;                   // Defensive: skip nulls.
        std::stringstream ss;
        ss << "ITEM,"
           << item->getId() << ","                       // ID (no commas allowed).
           << escapeField(item->getName()) << ","        // Name (escape commas).
           << item->typeTag() << ","                     // "Weapon"/"Armor"/"Potion"
           << item->getBasePrice() << ","
           << (item->isAvailable() ? 1 : 0) << ","
           << escapeField(item->getDescription()) << ",";

        // Type-specific stats. dynamic_cast safely tests "is this object
        // really a Weapon* underneath?"; returns nullptr if not.
        if (auto* w = dynamic_cast<Weapon*>(item)) {
            ss << w->getDamage() << "," << w->getDurability();
        } else if (auto* a = dynamic_cast<Armor*>(item)) {
            ss << a->getDefense() << "," << a->getDurability();
        } else if (auto* p = dynamic_cast<Potion*>(item)) {
            ss << p->getHealingAmount() << ","
               << (p->isConsumedOnUse() ? 1 : 0);
        } else {
            ss << "0,0";                       // Unknown type - placeholder.
        }
        file << ss.str() << "\n";
    }

    // ---- RENTAL lines ----
    for (const auto& r : rentals) {
        std::stringstream ss;
        ss << "RENTAL,"
           << r.getRecordId() << ","
           << escapeField(r.getCustomerName()) << ","
           // If the rental's item pointer is null, write "NONE" so we
           // can detect it during load and leave the record unlinked.
           << (r.getRentedItem() ? r.getRentedItem()->getId() : "NONE") << ","
           << r.getRentalDay() << ","
           << r.getRentalDuration() << ","
           << r.getTotalFee();
        file << ss.str() << "\n";
    }

    file.close();                              // Flush and close.
    if (file.fail()) {                         // Check for write errors.
        throw FileAccessException(
            "Write to '" + filename + "' failed mid-stream.");
    }
}
