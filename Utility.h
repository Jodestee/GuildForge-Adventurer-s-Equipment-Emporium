// =====================================================================
//  File:     Utility.h
//  Project:  GuildForge: Adventurer's Equipment Emporium
//  Course:   CSCI 272 - Object-Oriented Programming in C++
//  Group:    Group 2 - "The Boys"
//  Owner:    Jn Francois, Jodestee
//  Purpose:  Header-only generic search templates. These satisfy the
//            "templates" rubric requirement AND get used in real game
//            logic (looking up items in inventory and rentals by ID).
// =====================================================================

#ifndef UTILITY_H
#define UTILITY_H

#include <vector>                    // We search std::vectors.
#include <string>                    // (Most attribute lookups use strings.)
#include <functional>                // (For documentation; we accept any callable.)

// ---------------------------------------------------------------------
// searchByAttribute<T, AttrType, Getter>
//
//   Finds the first element in 'collection' for which getter(element)
//   equals 'target'. Returns a pointer to that element, or nullptr.
//
//   Three template parameters:
//     T        - the element type stored in the vector.
//     AttrType - the type of the attribute we're matching (string, int...).
//     Getter   - any callable (lambda, function pointer, functor) that,
//                given a T, returns its attribute.
//
//   Example call:
//       RentalRecord* r = searchByAttribute<RentalRecord, std::string>(
//           rentals, "R007",
//           [](const RentalRecord& rr){ return rr.getRecordId(); });
// ---------------------------------------------------------------------
template <typename T, typename AttrType, typename Getter>
T* searchByAttribute(std::vector<T>& collection,
                     const AttrType& target,
                     Getter getter) {
    for (auto& element : collection) {                  // Walk every element.
        if (getter(element) == target) {                // Attribute matches?
            return &element;                            // Return its address.
        }
    }
    return nullptr;                                     // Not found.
}

// ---------------------------------------------------------------------
// searchPointerByAttribute<T, AttrType, Getter>
//
//   Convenience overload for vectors of pointers (like
//   std::vector<Equipment*>). Without it, the basic template would
//   return Equipment** (pointer to a pointer in the vector), which
//   is awkward to use. This version returns the matching pointer
//   directly.
// ---------------------------------------------------------------------
template <typename T, typename AttrType, typename Getter>
T* searchPointerByAttribute(std::vector<T*>& collection,
                            const AttrType& target,
                            Getter getter) {
    for (auto* element : collection) {                  // Walk each pointer.
        if (getter(element) == target) {                // Attribute matches?
            return element;                             // Return the pointer.
        }
    }
    return nullptr;                                     // Not found.
}

#endif // UTILITY_H
