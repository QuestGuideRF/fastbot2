#pragma once
#include <Arduino.h>
#include <GSON.h>
#include <StringUtils.h>
#include "../EntryAccess.h"
#include "../api.h"
#include "ChatRead.h"
#include "DocumentRead.h"
#include "LocationRead.h"
#include "MessageOriginRead.h"
#include "UserRead.h"
namespace fb {
struct MessageRead : public EntryAccess {
    MessageRead(EntryAccess entry) : EntryAccess(entry) {}
    Text text() {
        return entry[tg_apih::text];
    }
    Text caption() {
        return entry[tg_apih::caption];
    }
    Text id() {
        return entry[tg_apih::message_id];
    }
    Text threadID() {
        return entry[tg_apih::message_thread_id];
    }
    bool isTopic() {
        return entry[tg_apih::is_topic_message] == true;
    }
    Text date() {
        return entry[tg_apih::date];
    }
    Text editDate() {
        return entry[tg_apih::edit_date];
    }
    UserRead from() {
        return UserRead(entry[tg_apih::from]);
    }
    UserRead viaBot() {
        return UserRead(entry[tg_apih::via_bot]);
    }
    ChatRead chat() {
        return ChatRead(entry[tg_apih::chat]);
    }
    ChatRead senderChat() {
        return ChatRead(entry[tg_apih::sender_chat]);
    }
    bool isReply() {
        return entry.has(tg_apih::reply_to_message);
    }
    MessageRead reply() {
        return MessageRead(entry[tg_apih::reply_to_message]);
    }
    bool isForward() {
        return entry.has(tg_apih::forward_origin);
    }
    MessageOriginRead forward() {
        return MessageOriginRead(entry[tg_apih::forward_origin]);
    }
    bool hasLocation() {
        return entry.has(tg_apih::location);
    }
    LocationRead location() {
        return LocationRead(entry[tg_apih::location]);
    }
    bool hasDocument() {
        return entry.has(tg_apih::document);
    }
    DocumentRead document() {
        return DocumentRead(entry[tg_apih::document]);
    }
};
}