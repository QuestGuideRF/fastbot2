#pragma once
#include <Arduino.h>
#include <GSON.h>
#include <StringUtils.h>
#include "../api.h"
#include "../packet.h"
#include "FastBot2Client_class.h"
#include "ID.h"
#include "InlineMenu.h"
#include "Menu.h"
#include "Message_class.h"
namespace fb {
struct ReplyParam {
    int32_t messageID = -1;
    ID chatID;
};
class Message {
    friend class ::FastBot2Client;
   public:
    enum class Mode : uint8_t {
        Text,
        MarkdownV2,
        HTML,
    };
    Message() {}
    Message(const Text& text, ID chatID) : chatID(chatID) {
        text.toString(this->text);
    }
    gson::Str json;
    String text;
    ID chatID;
    int32_t threadID = -1;
    ReplyParam reply;
    bool preview = previewDefault;
    bool notification = notificationDefault;
    bool protect = protectDefault;
    Mode mode = modeDefault;
    void setMenu(Menu& menu) {
        if (menu.text.length()) _menu = &menu;
    }
    void setInlineMenu(InlineMenu& menu) {
        if (menu.text.length() && menu.data.length()) _menu_inline = &menu;
    }
    void removeMenu() {
        _remove_menu = 1;
    }
    void setModeMD() {
        mode = Mode::MarkdownV2;
    }
    void setModeHTML() {
        mode = Mode::HTML;
    }
    static bool previewDefault;
    static bool notificationDefault;
    static bool protectDefault;
    static Mode modeDefault;
   private:
    bool _remove_menu = 0;
    Menu* _menu = nullptr;
    InlineMenu* _menu_inline = nullptr;
   protected:
    void makePacket(Packet& p) const {
        p[tg_api::chat_id] = chatID;
        if (text.length()) p[tg_api::text].escape(text);
        if (threadID >= 0) p[tg_api::message_thread_id] = threadID;
        if (reply.messageID >= 0) {
            p[tg_api::reply_parameters]('{');
            p[tg_api::message_id] = reply.messageID;
            if (reply.chatID) p[tg_api::chat_id] = reply.chatID;
            p('}');
        }
        if (!preview) {
            p[tg_api::link_preview_options]('{');
            p[tg_api::is_disabled] = true;
            p('}');
        }
        if (!notification) p[tg_api::disable_notification] = true;
        if (protect) p[tg_api::protect_content] = true;
        if (mode != Message::Mode::Text) p[tg_api::parse_mode] = (mode == Message::Mode::MarkdownV2 ? F("MarkdownV2") : F("HTML"));
        if (_remove_menu || _menu_inline || _menu) {
            p[tg_api::reply_markup]('{');
            makeMenu(p);
            p('}');
        }
        p += json;
    }
    void makeQS(Packet& p) const {
        p.addQS(tg_api::chat_id, chatID);
        if (text.length()) p.addQS(tg_api::text, su::url::encode(text));
        if (threadID >= 0) p.addQS(tg_api::message_thread_id, threadID);
        if (reply.messageID >= 0) {
            p.beginQS(tg_api::reply_parameters);
            p('{');
            p[tg_api::message_id] = reply.messageID;
            if (reply.chatID) p[tg_api::chat_id] = reply.chatID;
            p('}');
        }
        if (!preview) {
            p.beginQS(tg_api::link_preview_options);
            p('{');
            p[tg_api::is_disabled] = true;
            p('}');
        }
        if (!notification) p.addQS(tg_api::disable_notification, true);
        if (protect) p.addQS(tg_api::protect_content, true);
        if (mode != Message::Mode::Text) p.addQS(tg_api::parse_mode, mode == (Message::Mode::MarkdownV2) ? F("MarkdownV2") : F("HTML"));
        if (_remove_menu || _menu_inline || _menu) {
            p.beginQS(tg_api::reply_markup);
            p('{');
            makeMenu(p);
            p('}');
        }
    }
    void makeMenu(Packet& p) const {
        if (_remove_menu) p[tg_api::remove_keyboard] = true;
        else if (_menu_inline) _menu_inline->_toJson(p);
        else if (_menu) _menu->_toJson(p);
    }
};
}