#pragma once
#include <Arduino.h>
#include <GSON.h>
#include <StringUtils.h>
#include "../EntryAccess.h"
#include "../api.h"
namespace fb {
struct UserRead : public EntryAccess {
    UserRead(gson::Entry entry) : EntryAccess(entry) {}
    Text id() {
        return entry[tg_apih::id];
    }
    bool isBot() {
        return entry[tg_apih::is_bot] == true;
    }
    Text firstName() {
        return entry[tg_apih::first_name];
    }
    Text lastName() {
        return entry[tg_apih::last_name];
    }
    Text username() {
        return entry[tg_apih::username];
    }
    Text languageCode() {
        return entry[tg_apih::language_code];
    }
    bool isPremium() {
        return entry[tg_apih::is_premium] == true;
    }
};
}