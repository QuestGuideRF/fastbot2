#pragma once
#include <Arduino.h>
#include <GSON.h>
#include <StringUtils.h>
#include "Message_class.h"
#include "../api.h"
#include "../packet.h"
namespace fb {
class Menu {
    friend class Message;
   public:
    Menu() {}
    Menu(Text text) {
        text.toString(this->text);
    }
    String text = "";
    String placeholder = "";
    bool persistent = persistentDefault;
    bool resize = resizeDefault;
    bool oneTime = oneTimeDefault;
    bool selective = selectiveDefault;
    Menu& addButton(Text text) {
        if (_first) _first = false;
        else this->text += ';';
        text.addString(this->text);
        return *this;
    }
    Menu& newRow() {
        text += '\n';
        _first = true;
        return *this;
    }
    static bool persistentDefault;
    static bool resizeDefault;
    static bool oneTimeDefault;
    static bool selectiveDefault;
   private:
    bool _first = true;
    void _toJson(Packet& p) {
        p[tg_api::keyboard]('[');
        su::TextParser rows(text, '\n');
        while (rows.parse()) {
            su::TextParser cols(rows, ';');
            p('[');
            while (cols.parse()) p.escape(cols);
            p(']');
        }
        p(']');
        if (persistent) p[tg_api::is_persistent] = true;
        if (resize) p[tg_api::resize_keyboard] = true;
        if (oneTime) p[tg_api::one_time_keyboard] = true;
        if (selective) p[tg_api::selective] = true;
        if (placeholder.length()) p[tg_api::input_field_placeholder].escape(placeholder);
    }
};
}