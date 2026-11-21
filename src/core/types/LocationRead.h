#pragma once
#include <Arduino.h>
#include <GSON.h>
#include <StringUtils.h>
#include "../EntryAccess.h"
#include "../api.h"
namespace fb {
struct LocationRead : public EntryAccess {
    LocationRead(gson::Entry entry) : EntryAccess(entry) {}
    Text latitude() {
        return entry[tg_apih::latitude];
    }
    Text longitude() {
        return entry[tg_apih::longitude];
    }
    Text horizontalAccuracy() {
        return entry[tg_apih::horizontal_accuracy];
    }
    Text livePeriod() {
        return entry[tg_apih::live_period];
    }
    Text heading() {
        return entry[tg_apih::heading];
    }
    Text proximityAlertRadius() {
        return entry[tg_apih::proximity_alert_radius];
    }
};
}