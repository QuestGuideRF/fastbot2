#pragma once
#include <Arduino.h>
#include <GSON.h>
#include <StringUtils.h>
#include "ChatRead.h"
#include "UserRead.h"
#include "../EntryAccess.h"
#include "../api.h"
namespace fb {
struct MessageOriginRead : public EntryAccess {
    MessageOriginRead(gson::Entry entry) : EntryAccess(entry) {}
    enum class Type : size_t {
        user = SH("user"),
        hiddenUser = SH("hidden_user"),
        chat = SH("chat"),
        channel = SH("channel"),
    };
    Type type() {
        return (Type)entry[tg_apih::type].hash();
    }
    Text date() {
        return entry[tg_apih::date];
    }
    UserRead senderUser() {
        return UserRead(entry[tg_apih::sender_user]);
    }
    ChatRead senderChat() {
        return ChatRead(entry[tg_apih::sender_chat]);
    }
    ChatRead chat() {
        return ChatRead(entry[tg_apih::chat]);
    }
};
}