// =====================================================================
//  File:     GuildManager.cpp
//  Project:  GuildForge: Adventurer's Equipment Emporium
//  Course:   CSCI 272 - Object-Oriented Programming in C++
//  Group:    Group 2 - "The Boys"
//  Owner:    Jn Francois, Jodestee
//  Purpose:  Implements the controller. This file ties every other
//            class together: it uses Equipment*, calls Weapon/Armor/
//            Potion through virtual dispatch, fires the searchByAttribute
//            template, hosts the try/catch blocks, and drives the
//            menu loop the player actually interacts with.
// =====================================================================

#include "GuildManager.h"
#include "Weapon.h"                  // We instantiate Weapons in createNewGuild().
#include "Armor.h"
#include "Potion.h"
#include "FileHandler.h"             // For load/save.
#include "Utility.h"                 // searchByAttribute / searchPointerByAttribute.
#include "CustomExceptions.h"        // Our three custom exceptions.
#include "Visuals.h"                 // Color codes and themed printing.

#include <iostream>
#include <iomanip>                   // std::fixed / std::setprecision / std::setw.
#include <sstream>                   // std::ostringstream for building IDs.
#include <cstdlib>                   // std::rand / std::srand.
#include <ctime>                     // std::time for seeding the RNG.
#include <limits>                    // (Reserved for future input cleanup helpers.)

using namespace Visuals;             // Lets us write Color::GOLD instead of
                                     // Visuals::Color::GOLD throughout the file.

// =====================================================================
//  Construction / destruction
// =====================================================================

GuildManager::GuildManager(const std::string& saveFile)
    : gold(0.0), reputation(0), currentDay(1),
      nextRecordNumber(1), saveFile(saveFile) {
    // Seed the C random number generator with current time so each run
    // produces different random events, customer choices, and rolls.
    std::srand(static_cast<unsigned>(std::time(nullptr)));
}

GuildManager::~GuildManager() {
    // We own the Equipment* objects in 'inventory' - free them.
    // Because Equipment has a virtual destructor, this correctly runs
    // ~Weapon() / ~Armor() / ~Potion() via dynamic dispatch.
    for (Equipment* item : inventory) {
        delete item;
    }
    inventory.clear();
}

// =====================================================================
//  Public entry point
// =====================================================================

void GuildManager::run() {
    clearScreen();                   // Wipe the terminal for a clean start.
    printTitleBanner();              // Show the gold ASCII title art.

    // ---- Initial load (fall back to a fresh guild on any failure) ----
    try {
        loadFromDisk();
    } catch (const FileAccessException& e) {
        // Domain-specific exception from our FileHandler.
        error(e.what());
        warn("Starting a fresh guild instead.");
        createNewGuild();
    } catch (const std::exception& e) {
        // Anything unexpected from the standard library.
        error(std::string("Unexpected error during load: ") + e.what());
        warn("Starting a fresh guild instead.");
        createNewGuild();
    }

    // ---- Main game loop ----
    bool running = true;             // saveAndQuit() flips this to false.
    while (running) {
        displayMainMenu();           // Re-print the menu every iteration.

        prompt("Choose an option (1-7): ");
        std::string input;
        std::getline(std::cin, input);   // Read the full input line.

        // ---- Wrap input parsing + dispatch in try/catch ----
        // This is one of the rubric-required try/catch blocks. It catches
        // (a) bad numeric input, (b) out-of-range menu choices, and
        // (c) propagated exceptions from inside the option handlers.
        try {
            if (input.empty()) {
                throw std::out_of_range("No input provided.");
            }
            int choice = std::stoi(input);   // Throws invalid_argument if non-numeric.

            switch (choice) {
                case 1: viewInventory();       break;
                case 2: rentEquipment();       break;
                case 3: returnItem();          break;
                case 4: handleRandomEvent();   break;
                case 5: viewActiveRentals();   break;
                case 6: viewDailyReport();     break;
                case 7: saveAndQuit(running);  break;
                default:
                    throw std::out_of_range("Choice must be between 1 and 7.");
            }
        }
        // Catch order matters: most specific first, most general last.
        catch (const std::invalid_argument&) {
            error("That doesn't look like a number. Try again.");
        }
        catch (const std::out_of_range& e) {
            error(e.what());
        }
        catch (const InvalidItemID& e) {        // Our custom exception.
            error(e.what());
        }
        catch (const NegativeGoldException& e) {
            error(e.what());
        }
        catch (const std::exception& e) {       // Catch-all safety net.
            error(std::string("Something went wrong: ") + e.what());
        }
    }
}

// =====================================================================
//  Loading + first-run initialization
// =====================================================================

void GuildManager::loadFromDisk() {
    // Ask FileHandler to read everything.
    GuildState state = FileHandler::loadFromFile(saveFile);

    if (!state.fileExisted) {
        info("No save file found - starting a brand new guild.");
        pause(400);
        createNewGuild();
        return;
    }

    // Move the loaded data into our member fields.
    inventory      = state.inventory;
    activeRentals  = state.rentals;
    gold           = state.gold;
    reputation     = state.reputation;
    currentDay     = state.currentDay;

    // Figure out where to start numbering new rentals so we don't reuse
    // an ID like R007 if one already exists in the file.
    nextRecordNumber = 1;
    for (const auto& r : activeRentals) {
        try {
            // Record IDs look like "R007"; skip the 'R' and parse the rest.
            if (r.getRecordId().size() > 1 && r.getRecordId()[0] == 'R') {
                int n = std::stoi(r.getRecordId().substr(1));
                if (n >= nextRecordNumber) nextRecordNumber = n + 1;
            }
        } catch (...) {
            // A weird ID we can't parse - just ignore it.
        }
    }

    success("Guild data loaded from " + saveFile);
    info("Welcome back, Guildmaster! It is day " +
         std::to_string(currentDay) + ".");
    pause(400);
}

void GuildManager::createNewGuild() {
    printSection("Welcome, broke peasant!", Color::B_YELLOW);
    info("You have inherited an empty shop. We've stocked you with starter gear.");

    // ---- Starter weapons ----
    inventory.push_back(new Weapon(
        "W001", "Silver Sword", 45.0, true,
        "Steel blade that glows faintly in moonlight", 25, 30));
    inventory.push_back(new Weapon(
        "W002", "Oak Bow", 35.0, true,
        "Reliable bow carved from ancient oak", 18, 40));
    inventory.push_back(new Weapon(
        "W003", "Iron Battleaxe", 55.0, true,
        "Heavy-headed axe favored by mountain clans", 32, 25));

    // ---- Starter armor ----
    inventory.push_back(new Armor(
        "A001", "Leather Vest", 30.0, true,
        "Basic protection for hesitant adventurers", 15, 20));
    inventory.push_back(new Armor(
        "A002", "Chainmail Hauberk", 50.0, true,
        "Interlocking iron rings, reliably solid", 28, 35));
    inventory.push_back(new Armor(
        "A003", "Tower Shield", 40.0, true,
        "Bulky shield wide enough to hide behind", 22, 40));

    // ---- Starter potions ----
    inventory.push_back(new Potion(
        "P001", "Health Potion", 15.0, true,
        "Crimson liquid that mends wounds", 50, true));
    inventory.push_back(new Potion(
        "P002", "Greater Healing Draught", 35.0, true,
        "Bubbling pink elixir of legend", 100, true));
    inventory.push_back(new Potion(
        "P003", "Stamina Tonic", 10.0, true,
        "Mint-flavored, restores a little vigor", 25, true));

    gold             = 100.0;        // Starter treasury.
    reputation       = 0;
    currentDay       = 1;
    nextRecordNumber = 1;

    success("Starter inventory issued. Treasury: $100.");
    pause(500);
}

// =====================================================================
//  Menu rendering
// =====================================================================

void GuildManager::displayMainMenu() const {
    printMenuHeader("M A I N   M E N U");

    // Status bar: day | gold | reputation, all colorized.
    std::cout << Color::INFO
              << "  Day " << currentDay
              << "   |   Gold: " << Color::GOLD << "$" << std::fixed
              << std::setprecision(2) << gold << Color::INFO
              << "   |   Reputation: " << Color::B_MAGENTA << reputation
              << Color::RESET << "\n\n";

    // Each menu option: magenta number, plain label.
    std::cout << Color::MENU << "  [1]" << Color::RESET << "  View Inventory\n";
    std::cout << Color::MENU << "  [2]" << Color::RESET << "  Rent Equipment\n";
    std::cout << Color::MENU << "  [3]" << Color::RESET << "  Return Item\n";
    std::cout << Color::MENU << "  [4]" << Color::RESET << "  Random Event / Quest\n";
    std::cout << Color::MENU << "  [5]" << Color::RESET << "  View Active Rentals\n";
    std::cout << Color::MENU << "  [6]" << Color::RESET << "  Daily Report\n";
    std::cout << Color::MENU << "  [7]" << Color::RESET << "  Save & Quit\n";
    divider('-', 60, Color::B_CYAN);
}

// =====================================================================
//  Option 1 - View Inventory
// =====================================================================
void GuildManager::viewInventory() const {
    printSection("INVENTORY", Color::B_CYAN);
    if (inventory.empty()) {
        info("The shop is empty.");
        return;
    }
    // POLYMORPHISM IN ACTION:
    // Even though 'item' is an Equipment*, the right override
    // (Weapon::displayDetails / Armor::displayDetails / Potion::displayDetails)
    // runs at runtime via virtual dispatch.
    for (Equipment* item : inventory) {
        item->displayDetails();
        std::cout << "\n";
    }
}

// =====================================================================
//  Option 2 - Rent Equipment
// =====================================================================
void GuildManager::rentEquipment() {
    printSection("RENT EQUIPMENT", Color::B_YELLOW);

    // Pick a random NPC customer name for flavor.
    auto names = sampleCustomerNames();
    std::string customer = names[std::rand() % names.size()];
    std::cout << "  A customer arrives: " << Color::BOLD << customer
              << Color::RESET << "\n";
    std::cout << "  \"I need some gear for the road. What's available?\"\n\n";

    prompt("Enter the item ID to rent (or 'cancel'): ");
    std::string id;
    std::getline(std::cin, id);

    if (id == "cancel" || id.empty()) {
        info("Rental cancelled. The customer leaves disappointed.");
        return;
    }

    // TEMPLATE IN ACTION: searchPointerByAttribute<> finds the matching
    // Equipment* by its ID, or returns nullptr if no match exists.
    Equipment* item = searchPointerByAttribute<Equipment, std::string>(
        inventory, id,
        [](Equipment* e){ return e->getId(); });

    if (!item) {
        // CUSTOM EXCEPTION IN ACTION: thrown here, caught in run().
        throw InvalidItemID(id);
    }
    if (!item->isAvailable()) {
        warn(item->getName() + " is already rented out.");
        return;
    }

    // Ask how many days the customer wants the item.
    prompt("How many days will they rent it for? ");
    std::string durStr;
    std::getline(std::cin, durStr);
    int duration;
    try {
        duration = std::stoi(durStr);
    } catch (...) {
        warn("Invalid duration - defaulting to 1 day.");
        duration = 1;
    }
    if (duration < 1) duration = 1;

    // ---- Process the rental ----
    item->rent();                                                // Mark unavailable.
    RentalRecord record(nextRecordId(), customer, item,
                        currentDay, duration);
    double upfrontFee = item->getBasePrice() * duration;
    record.setTotalFee(upfrontFee);
    activeRentals.push_back(record);                             // Save the record.

    gold       += upfrontFee;        // Collect gold up front.
    reputation += 1;                 // Tiny reputation bump for completing a rental.
    currentDay += 1;                 // Advance time.

    success(customer + " rents " + item->getName() +
            " for " + std::to_string(duration) + " day(s).");
    info("Earned $" + std::to_string(static_cast<int>(upfrontFee)) +
         " upfront. Reputation +1. Day advances.");
}

// =====================================================================
//  Option 3 - Return Item
// =====================================================================
void GuildManager::returnItem() {
    printSection("RETURN ITEM", Color::B_RED);

    if (activeRentals.empty()) {
        info("There are no active rentals.");
        return;
    }

    prompt("Enter the rental record ID (e.g. R001): ");
    std::string recId;
    std::getline(std::cin, recId);

    RentalRecord* rec = findRentalById(recId);   // Uses the template internally.
    if (!rec) {
        throw InvalidItemID(recId);              // Custom exception.
    }

    // Calculate fee + check overdue (this is where calculateFee +
    // isOverdue + operator+ all show up).
    double finalFee = rec->calculateFee(currentDay);
    bool   overdue  = rec->isOverdue(currentDay);

    // OPERATOR+ DEMO: build a running total starting from 0 and add
    // this rental's fee to it. Demonstrates the overload in real use.
    RentalRecord runningTotal;
    runningTotal.setTotalFee(0.0);
    RentalRecord merged = runningTotal + (*rec);  // operator+ called here.

    // Mark the underlying item available again (if still linked).
    if (rec->getRentedItem()) {
        rec->getRentedItem()->returnItem();
        success(rec->getRentedItem()->getName() + " returned by " +
                rec->getCustomerName() + ".");
    } else {
        warn("Item pointer was unlinked, but rental cleared.");
    }

    // Apply outcome.
    if (overdue) {
        double extra = finalFee - rec->getTotalFee();
        if (extra < 0) extra = 0;
        warn("Returned LATE! Overdue surcharge: $" +
             std::to_string(static_cast<int>(extra)));
        gold       += extra;
        reputation -= 1;
    } else {
        info("Returned on time. Reputation +1.");
        reputation += 1;
    }

    info("Daily merged revenue total (via operator+): $" +
         std::to_string(static_cast<int>(merged.getTotalFee())));

    // Erase the closed rental record.
    for (auto it = activeRentals.begin(); it != activeRentals.end(); ++it) {
        if (it->getRecordId() == recId) {
            activeRentals.erase(it);
            break;
        }
    }
    currentDay += 1;
}

// =====================================================================
//  Option 4 - Random Event
// =====================================================================
void GuildManager::handleRandomEvent() {
    printSection("RANDOM EVENT", Color::B_MAGENTA);

    if (activeRentals.empty()) {
        info("No adventurers are out in the field. Nothing happens today.");
        return;
    }

    // Pick a random active rental to "trigger" an event on.
    int idx = std::rand() % activeRentals.size();
    RentalRecord& rec = activeRentals[idx];

    if (!rec.getRentedItem()) {
        warn("Rental " + rec.getRecordId() + " has no linked item. Skipped.");
        return;
    }

    std::cout << "  " << Color::BOLD << rec.getCustomerName() << Color::RESET
              << " is using " << Color::BOLD
              << rec.getRentedItem()->getName() << Color::RESET
              << " in the field...\n";
    loadingDots("  Rolling dice", 4, 200);     // Suspense pause.

    int roll = std::rand() % 100;              // 0..99 roll.

    if (roll < 70) {
        // --- 70% chance: quest success ---
        std::cout << Color::SUCCESS << "  >> QUEST SUCCESS! <<"
                  << Color::RESET << "\n";

        // POLYMORPHISM IN ACTION: useItem() on Equipment* dispatches to
        // Weapon::useItem / Armor::useItem / Potion::useItem at runtime.
        std::string result = rec.getRentedItem()->useItem();
        std::cout << "  " << Color::ITEM << result << Color::RESET << "\n";

        double bounty = 20.0 + (std::rand() % 50);  // Random reward.
        gold       += bounty;
        reputation += 2;
        success("Earned $" + std::to_string(static_cast<int>(bounty)) +
                " bounty. Reputation +2.");
    } else {
        // --- 30% chance: disaster ---
        std::cout << Color::ERROR << "  >> DISASTER! <<"
                  << Color::RESET << "\n";
        std::cout << "  " << rec.getRentedItem()->getName()
                  << " was lost or broken in the field.\n";
        rec.getRentedItem()->returnItem();      // Free up the item slot.
        reputation -= 2;
        warn("Reputation -2. The customer is unhappy.");
    }
    currentDay += 1;
}

// =====================================================================
//  Option 5 - View Active Rentals
// =====================================================================
void GuildManager::viewActiveRentals() const {
    printSection("ACTIVE RENTALS", Color::B_MAGENTA);
    if (activeRentals.empty()) {
        info("No active rentals.");
        return;
    }
    for (const auto& r : activeRentals) {
        r.displayRecord();
        if (r.isOverdue(currentDay)) {
            warn("  ^ OVERDUE on this rental!");
        }
        std::cout << "\n";
    }
}

// =====================================================================
//  Option 6 - Daily Report
// =====================================================================
void GuildManager::viewDailyReport() const {
    printSection("DAILY REPORT", Color::B_CYAN);
    std::cout << "  Day:        " << Color::BOLD << currentDay
              << Color::RESET << "\n";
    std::cout << "  Gold:       " << Color::GOLD << "$" << std::fixed
              << std::setprecision(2) << gold << Color::RESET << "\n";
    std::cout << "  Reputation: " << Color::B_MAGENTA << reputation
              << Color::RESET << "\n";
    std::cout << "  Inventory:  " << inventory.size() << " items\n";
    std::cout << "  Active rentals: " << activeRentals.size() << "\n";

    // OPERATOR+ DEMO #2: chain operator+ across every active rental
    // to compute the total committed revenue.
    if (!activeRentals.empty()) {
        RentalRecord total;
        total.setTotalFee(0.0);
        for (const auto& r : activeRentals) {
            total = total + r;       // operator+ called once per rental.
        }
        std::cout << "  Outstanding rental value: " << Color::GOLD << "$"
                  << std::fixed << std::setprecision(2)
                  << total.getTotalFee() << Color::RESET << "\n";
    }
}

// =====================================================================
//  Option 7 - Save & Quit
// =====================================================================
void GuildManager::saveAndQuit(bool& running) {
    printSection("SAVE & QUIT", Color::B_GREEN);
    try {
        // FileHandler does the actual disk work and may throw.
        FileHandler::saveToFile(saveFile, inventory, activeRentals,
                                gold, reputation, currentDay);
        success("Guild data saved to " + saveFile);
    } catch (const FileAccessException& e) {
        // Save failed - tell the user and let them decide whether to
        // quit anyway (losing progress) or stay in the game.
        error(e.what());
        prompt("Quit anyway? (y/n): ");
        std::string answer;
        std::getline(std::cin, answer);
        if (answer != "y" && answer != "Y") {
            warn("Save & Quit cancelled.");
            return;
        }
    }
    cprintln("\n  Farewell, Guildmaster. May your treasury overflow.\n",
             Color::B_YELLOW);
    running = false;                 // This breaks out of the main loop.
}

// =====================================================================
//  Helpers
// =====================================================================

// Returns "R001", "R002", ... and increments the counter.
std::string GuildManager::nextRecordId() {
    std::ostringstream ss;
    // setw(3) + setfill('0') give us zero-padded width-3 output.
    ss << "R" << std::setw(3) << std::setfill('0') << nextRecordNumber++;
    return ss.str();
}

// Wrapper: search inventory by ID using the template.
Equipment* GuildManager::findItemById(const std::string& id) {
    return searchPointerByAttribute<Equipment, std::string>(
        inventory, id,
        [](Equipment* e){ return e->getId(); });
}

// Wrapper: search activeRentals by record ID using the template.
RentalRecord* GuildManager::findRentalById(const std::string& id) {
    return searchByAttribute<RentalRecord, std::string>(
        activeRentals, id,
        [](const RentalRecord& r){ return r.getRecordId(); });
}

// A small bank of NPC names. rentEquipment() picks one at random.
std::vector<std::string> GuildManager::sampleCustomerNames() const {
    return {
        "Lyra Brightwing", "Garrick Stoneshield", "Mira Thornwood",
        "Dorin Ashbrand",  "Selene Moonweaver",   "Borin the Bold",
        "Kyra Shadowstep", "Tomas Ironheart",     "Vex Nightshade"
    };
}
