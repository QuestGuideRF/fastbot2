#pragma once
#include <Arduino.h>
#include <GSON.h>
namespace fb {
class EntryAccess {
   public:
    EntryAccess(const gson::Entry& entry) : entry(entry) {}
    bool has(size_t hash) const {
        return entry[hash].valid();
    }
    gson::Entry operator[](size_t hash) const {
        return entry[hash];
    }
    gson::Entry operator[](int index) const {
        return entry[index];
    }
    gson::Entry entry;
    bool includes(size_t hash) const __attribute__((deprecated)) {
        return has(hash);
    }
};
}