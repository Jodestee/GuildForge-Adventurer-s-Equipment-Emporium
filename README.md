# GuildForge: Adventurer's Equipment Emporium

**CSCI 272 Capstone — Group 2: "The Boys"**

A text-based RPG-flavored equipment rental simulator. The player runs a
fantasy guild shop, rents weapons / armor / potions to NPC adventurers,
handles random quests, and manages gold + reputation across save sessions.

---

## Team

| Member               | Role                                         |
|----------------------|----------------------------------------------|
| Jn Francois, Jodestee | Lead Architect / Integrator / Persistence   |
| Sharif, Yasin         | Inheritance Lead / Rental Records           |
| Vittini, Kelvin       | Derived Classes Lead                        |

---

## File ownership

| File                    | Owner                  | Notes                                       |
|-------------------------|------------------------|---------------------------------------------|
| `main.cpp`              | Jn Francois, Jodestee  | Entry point; constructs GuildManager        |
| `GuildManager.h/.cpp`   | Jn Francois, Jodestee  | Game loop, menus, exception handling        |
| `Utility.h`             | Jn Francois, Jodestee  | Template `searchByAttribute<T>`             |
| `CustomExceptions.h`    | Jn Francois, Jodestee  | 3 custom exception classes                  |
| `Visuals.h`             | Jn Francois, Jodestee  | ANSI colors, ASCII banner, themed printing  |
| `FileHandler.h/.cpp`    | Jn Francois, Jodestee  | CSV save/load with stringstream parsing     |
| `Equipment.h/.cpp`      | Sharif, Yasin          | Base class, `operator<<`, `operator==`      |
| `RentalRecord.h/.cpp`   | Sharif, Yasin          | Rental data + `operator+` overload          |
| `Weapon.h/.cpp`         | Vittini, Kelvin        | Derived class with damage + durability      |
| `Armor.h/.cpp`          | Vittini, Kelvin        | Derived class with defense + durability     |
| `Potion.h/.cpp`         | Vittini, Kelvin        | Derived class with healing + consumption    |
| `Makefile`              | Shared                 | Build script                                |
| `README.md`             | Shared                 | This file                                   |

---

## Build & run

Requires `g++` with C++17 support.

```bash
make          # builds ./guildforge
make run      # builds and runs
make clean    # removes binary, object files, and guild_save.csv
```

To run manually after building:

```bash
./guildforge
```

The first run creates a starter inventory and treasury of $100. Subsequent
runs load the previous state from `guild_save.csv`.

---

## Required-feature checklist

- [x] **Multi-file structure** — 13 files, every major class has its own .h/.cpp pair
- [x] **Inheritance** — Equipment base + Weapon, Armor, Potion derived classes
- [x] **Polymorphism** — virtual `displayDetails()` and `useItem()`, called through `Equipment*`
- [x] **Operator overloading** — `operator<<` and `operator==` on Equipment, `operator+` on RentalRecord
- [x] **Templates** — `searchByAttribute<T>` and `searchPointerByAttribute<T>` in Utility.h
- [x] **File I/O + stringstream** — FileHandler reads/writes guild_save.csv
- [x] **Persistence** — full guild state (inventory, rentals, gold, reputation, day) saved/loaded
- [x] **STL containers** — `vector<Equipment*>` and `vector<RentalRecord>`
- [x] **Exception handling** — custom `InvalidItemID`, `NegativeGoldException`, `FileAccessException` + multiple try/catch blocks
- [x] **Pointers** — base-class `Equipment*` central to polymorphism and dynamic creation
- [x] **Clean main()** — only constructs GuildManager and calls `run()`

---

## Visual polish

`Visuals.h` provides ANSI color codes, an ASCII title banner, themed
section headers, and `success / warn / error / info` helpers. All terminal
output routes through these so the game looks consistent and readable.

If your terminal does not support ANSI colors (rare on modern systems),
you'll see escape codes printed as text. Use any modern Linux/macOS
terminal, Windows Terminal, or VS Code's integrated terminal.

---

## Save file format (`guild_save.csv`)

Tagged lines, one record per line:

```
META,gold,reputation,currentDay
ITEM,id,name,type,price,available,description,stat1,stat2
RENTAL,recordId,customerName,itemId,rentalDay,duration,totalFee
```

Type-specific stat columns:
- **Weapon** → `damage, durability`
- **Armor** → `defense, durability`
- **Potion** → `healingAmount, consumedOnUse(0/1)`

Embedded commas in names/descriptions are escaped as `\;` and unescaped
on load. This keeps the parser simple while supporting flavor text like
"Interlocking iron rings, reliably solid".
