#pragma once
#include <Arduino.h>
#include <GSON.h>
#include <StringUtils.h>
#include "../EntryAccess.h"
#include "../api.h"
namespace fb {
struct ChatRead : public EntryAccess {
    ChatRead(gson::Entry entry) : EntryAccess(entry) {}
    enum class Type : size_t {
        privateChat = SH("private"),
        group = SH("group"),
        supergroup = SH("supergroup"),
        channel = SH("channel"),
    };
    Text id() {
        return entry[tg_apih::id];
    }
    Type type() {
        return (Type)entry[tg_apih::type].hash();
    }
    Text title() {
        return entry[tg_apih::title];
    }
    Text username() {
        return entry[tg_apih::username];
    }
    Text firstName() {
        return entry[tg_apih::first_name];
    }
    Text lastName() {
        return entry[tg_apih::last_name];
    }
    Text description() {
        return entry[tg_apih::description];
    }
    bool isForum() {
        return entry[tg_apih::is_forum] == true;
    }
};
}