// =====================================================================
//  File:     Visuals.h
//  Project:  GuildForge: Adventurer's Equipment Emporium
//  Course:   CSCI 272 - Object-Oriented Programming in C++
//  Group:    Group 2 - "The Boys"
//  Owner:    Jn Francois, Jodestee
//  Purpose:  Centralized visual helpers - ANSI color codes, ASCII title
//            banner, framed menu headers, dividers, themed result lines,
//            and pause/loading animations. Header-only (everything is
//            inline) so any .cpp can just #include it and use it.
// =====================================================================

#ifndef VISUALS_H                       // Header guard.
#define VISUALS_H

#include <iostream>                     // std::cout for printing.
#include <string>                       // std::string for message text.
#include <chrono>                       // std::chrono::milliseconds for timing.
#include <thread>                       // std::this_thread::sleep_for() for pauses.

namespace Visuals {                     // Wrap everything in a namespace so names
                                        // like 'pause' don't collide with std::pause.

    // -----------------------------------------------------------------
    // ANSI escape codes - these special character sequences tell modern
    // terminals to switch text color or style. They render as actual
    // colors on Linux/macOS terminals, Windows Terminal, and the VS Code
    // integrated terminal.
    // -----------------------------------------------------------------
    namespace Color {
        const std::string RESET   = "\033[0m";   // Turns off all styling.
        const std::string BOLD    = "\033[1m";   // Bold text.
        const std::string DIM     = "\033[2m";   // Dimmed (grey-ish) text.
        const std::string ITALIC  = "\033[3m";   // Italic text (not all terminals).
        const std::string UNDER   = "\033[4m";   // Underlined text.

        const std::string BLACK   = "\033[30m";  // Standard 8-color palette codes.
        const std::string RED     = "\033[31m";
        const std::string GREEN   = "\033[32m";
        const std::string YELLOW  = "\033[33m";
        const std::string BLUE    = "\033[34m";
        const std::string MAGENTA = "\033[35m";
        const std::string CYAN    = "\033[36m";
        const std::string WHITE   = "\033[37m";

        // "Bright" variants (90-series codes) look more vivid than the base set.
        const std::string B_RED     = "\033[91m";
        const std::string B_GREEN   = "\033[92m";
        const std::string B_YELLOW  = "\033[93m";
        const std::string B_BLUE    = "\033[94m";
        const std::string B_MAGENTA = "\033[95m";
        const std::string B_CYAN    = "\033[96m";
        const std::string B_WHITE   = "\033[97m";

        // Semantic aliases - the rest of the code uses these names so that if
        // we ever rethink the palette, we change it once here.
        const std::string GOLD    = B_YELLOW;    // Treasury / money.
        const std::string SUCCESS = B_GREEN;     // Good outcomes.
        const std::string ERROR   = B_RED;       // Errors.
        const std::string MENU    = B_MAGENTA;   // Menu option labels.
        const std::string INFO    = B_CYAN;      // Neutral info / status bar.
        const std::string ITEM    = WHITE;       // Plain item text.
        const std::string PROMPT  = B_YELLOW;    // Input prompts.
    }

    // -----------------------------------------------------------------
    // Print 'text' in 'color' and reset styling at the end. Inline so the
    // compiler can fold it into the call site without a real function call.
    // -----------------------------------------------------------------
    inline void cprint(const std::string& text, const std::string& color) {
        std::cout << color << text << Color::RESET;   // Wrap the text in color codes.
    }

    // Same as cprint but adds a newline at the end - convenience.
    inline void cprintln(const std::string& text, const std::string& color) {
        std::cout << color << text << Color::RESET << "\n";
    }

    // -----------------------------------------------------------------
    // Draws a horizontal divider, e.g. ========================.
    // Defaults to 60 '=' characters in cyan.
    // -----------------------------------------------------------------
    inline void divider(char ch = '=', int width = 60,
                        const std::string& color = Color::B_CYAN) {
        std::cout << color << std::string(width, ch)   // Build a string of 'width' chars.
                  << Color::RESET << "\n";
    }

    // -----------------------------------------------------------------
    // The big "GuildForge" ASCII art banner shown when the game starts.
    // Each line is printed in gold; the subtitle in magenta.
    // -----------------------------------------------------------------
    inline void printTitleBanner() {
        const std::string c = Color::GOLD;             // Local alias to keep lines short.
        std::cout << "\n";                             // Top spacer.
        // Each of the next six lines is one row of ASCII art for the title.
        std::cout << c << "  ____       _ _     _ _____                  " << Color::RESET << "\n";
        std::cout << c << " / ___|_   _(_) | __| |  ___|__  _ __ __ _  ___ " << Color::RESET << "\n";
        std::cout << c << "| |  _| | | | | |/ _` | |_ / _ \\| '__/ _` |/ _ \\" << Color::RESET << "\n";
        std::cout << c << "| |_| | |_| | | | (_| |  _| (_) | | | (_| |  __/" << Color::RESET << "\n";
        std::cout << c << " \\____|\\__,_|_|_|\\__,_|_|  \\___/|_|  \\__, |\\___|" << Color::RESET << "\n";
        std::cout << c << "                                     |___/      " << Color::RESET << "\n";
        cprintln("       Adventurer's Equipment Emporium",                 // Subtitle.
                 Color::B_MAGENTA);
        divider('=', 60, Color::B_CYAN);               // Cyan rule under the title.
    }

    // -----------------------------------------------------------------
    // Draws a "boxed" menu header with title centered between two cyan
    // dividers - e.g. used for the main menu and section headers.
    // -----------------------------------------------------------------
    inline void printMenuHeader(const std::string& title) {
        std::cout << "\n";                             // Spacer above.
        divider('=', 60, Color::B_CYAN);               // Top rule.
        std::cout << Color::B_MAGENTA << Color::BOLD;  // Bold magenta for title text.
        int pad = (60 - static_cast<int>(title.size())) / 2;  // Center math.
        if (pad < 0) pad = 0;                          // Don't allow negative pad.
        std::cout << std::string(pad, ' ') << title    // Padded title.
                  << Color::RESET << "\n";
        divider('=', 60, Color::B_CYAN);               // Bottom rule.
    }

    // -----------------------------------------------------------------
    // A lighter-weight section header (used inside menu options).
    // Format: "-- TITLE ----------------"
    // -----------------------------------------------------------------
    inline void printSection(const std::string& title,
                             const std::string& color = Color::B_CYAN) {
        std::cout << "\n" << color << "-- " << title << " "
                  << std::string(56 - static_cast<int>(title.size()), '-')
                  << Color::RESET << "\n";
    }

    // Print the prompt arrow + text in yellow without a trailing newline,
    // so std::getline reads the user's input on the same visual line.
    inline void prompt(const std::string& text) {
        std::cout << Color::PROMPT << " > " << text << Color::RESET;
    }

    // ----- Themed status helpers ------------------------------------
    // Each prints a small marker + the message in a semantic color.
    inline void success(const std::string& text) {
        std::cout << Color::SUCCESS << "  [OK] " << text << Color::RESET << "\n";
    }
    inline void warn(const std::string& text) {
        std::cout << Color::B_YELLOW << "  [!] "  << text << Color::RESET << "\n";
    }
    inline void error(const std::string& text) {
        std::cout << Color::ERROR   << "  [X] "  << text << Color::RESET << "\n";
    }
    inline void info(const std::string& text) {
        std::cout << Color::INFO    << "  [i] "  << text << Color::RESET << "\n";
    }

    // -----------------------------------------------------------------
    // Sleep the current thread briefly. Used for dramatic pauses
    // (for example, "rolling dice" during random events).
    // -----------------------------------------------------------------
    inline void pause(int milliseconds = 350) {
        std::this_thread::sleep_for(             // Standard library sleep helper.
            std::chrono::milliseconds(milliseconds));
    }

    // -----------------------------------------------------------------
    // Prints "label..." with each '.' appearing after a short delay.
    // Used to add suspense to random events.
    // -----------------------------------------------------------------
    inline void loadingDots(const std::string& label,
                            int dots = 3, int delayMs = 250) {
        std::cout << Color::DIM << label;        // Print the label dimmed.
        for (int i = 0; i < dots; ++i) {         // For each requested dot...
            std::cout.flush();                   // Force output to appear NOW.
            pause(delayMs);                      // Wait...
            std::cout << ".";                    // Print one dot.
        }
        std::cout << Color::RESET << "\n";       // Reset color and newline.
    }

    // -----------------------------------------------------------------
    // Best-effort terminal clear via ANSI codes:
    //   \033[2J  - clear entire screen
    //   \033[H   - move cursor to home position (top-left)
    // -----------------------------------------------------------------
    inline void clearScreen() {
        std::cout << "\033[2J\033[H";
    }

}  // namespace Visuals

#endif // VISUALS_H
