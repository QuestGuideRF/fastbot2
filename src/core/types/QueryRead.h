#pragma once
#include <Arduino.h>
#include <GSON.h>
#include <StringUtils.h>
#include "MessageRead.h"
#include "UserRead.h"
#include "../EntryAccess.h"
#include "../api.h"
namespace fb {
struct QueryRead : public EntryAccess {
    QueryRead(gson::Entry entry) : EntryAccess(entry) {}
    Text id() {
        return entry[tg_apih::id];
    }
    Text data() {
        return entry[tg_apih::data];
    }
    UserRead from() {
        return UserRead(entry[tg_apih::from]);
    }
    MessageRead message() {
        return MessageRead(entry[tg_apih::message]);
    }
};
}