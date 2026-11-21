#pragma once
#include <Arduino.h>
#include <GSON.h>
#include <StringUtils.h>
#include "FastBot2Client_class.h"
#include "../EntryAccess.h"
#include "../api.h"
namespace fb {
typedef void (*DownloadHook)(void* bot, const Text& id);
struct DocumentRead : public EntryAccess {
    DocumentRead(gson::Entry entry) : EntryAccess(entry) {}
    Text id() {
        return entry[tg_apih::file_id];
    }
    Text uniqueID() {
        return entry[tg_apih::file_unique_id];
    }
    Text name() {
        return entry[tg_apih::file_name];
    }
    Text type() {
        return entry[tg_apih::mime_type];
    }
    Text size() {
        return entry[tg_apih::file_size];
    }
};
}