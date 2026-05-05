// =====================================================================
//  File:     main.cpp
//  Project:  GuildForge: Adventurer's Equipment Emporium
//  Course:   CSCI 272 - Object-Oriented Programming in C++
//  Group:    Group 2 - "The Boys"
//  Owner:    Jn Francois, Jodestee
//  Purpose:  Program entry point. Per the design, main() does NOTHING
//            except construct a GuildManager and call run(). All real
//            game logic lives inside the manager class. The try/catch
//            here is only a last-resort net for fatal errors that
//            escape the manager.
// =====================================================================

#include "GuildManager.h"            // The controller class.
#include <iostream>                  // std::cerr for fatal-error output.
#include <exception>                 // std::exception base class.

int main() {
    try {
        GuildManager guild("guild_save.csv");   // Construct manager.
        guild.run();                            // Hand control over to it.
    }
    // ---- Last-resort exception handlers ----
    // If any exception escapes GuildManager::run() (it shouldn't, but
    // defense in depth), we report it and exit with a non-zero code so
    // shells / scripts can detect the failure.
    catch (const std::exception& e) {
        std::cerr << "\nFatal error: " << e.what() << "\n";
        return 1;
    }
    catch (...) {
        // The "..." catch handles any non-std::exception throw.
        std::cerr << "\nUnknown fatal error occurred.\n";
        return 1;
    }
    return 0;                        // Normal exit.
}
