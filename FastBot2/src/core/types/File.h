#pragma once
#include <Arduino.h>
#include <GSON.h>
#include <StringUtils.h>
#include "../Multipart.h"
#include "Message.h"
#if !defined(FB_NO_FILE) && (defined(ESP8266) || defined(ESP32))
namespace fb {
class File : protected Message {
    friend class ::FastBot2Client;
   public:
    enum class Type : uint8_t {
        photo,
        audio,
        document,
        video,
        animation,
        voice,
        video_note,
    };
#ifdef FS_H
    File(const Text& name, Type type, ::File& file) : File(name, type, file, false) {}
#endif
    File(const Text& name, Type type, const uint8_t* bytes, size_t length, bool pgm = false) : File(name, type, bytes, length, pgm, false) {}
    File(const Text& name, Type type, const Text& urlid) : File(name, type, urlid, false) {}
    using Message::chatID;
    using Message::json;
    using Message::mode;
    using Message::notification;
    using Message::protect;
    using Message::reply;
    using Message::setInlineMenu;
    using Message::threadID;
    String caption;
    Multipart multipart;
   protected:
    void makePacket(Packet& p) const {
        if (multipart.isFile()) {
            Message::makeQS(p);
            if (caption.length()) p.addQS(tg_api::caption, su::url::encode(caption));
        } else {
            Message::makePacket(p);
            p[multipart.getType()] = multipart.getUrlid();
            if (caption.length()) p[tg_api::caption].escape(caption);
        }
    }
#ifdef FS_H
    File(const Text& name, Type type, ::File& file, bool edit) : multipart(name, (Multipart::Type)type, file, edit) {}
#endif
    File(const Text& name, Type type, const uint8_t* bytes, size_t length, bool pgm, bool edit) : multipart(name, (Multipart::Type)type, bytes, length, pgm, edit) {}
    File(const Text& name, Type type, const Text& urlid, bool edit) : multipart(name, (Multipart::Type)type, urlid, edit) {}
};
class FileEdit : protected File {
    friend class ::FastBot2Client;
   public:
#ifdef FS_H
    FileEdit(const Text& name, Type type, ::File& file) : File(name, type, file, true) {}
#endif
    FileEdit(const Text& name, Type type, const uint8_t* bytes, size_t length, bool pgm = false) : File(name, type, bytes, length, pgm, true) {}
    FileEdit(const Text& name, Type type, const Text& urlid) : File(name, type, urlid, true) {}
    int32_t messageID;
    using File::caption;
    using File::chatID;
    using File::multipart;
    using Message::json;
    using Message::setInlineMenu;
   protected:
    void makePacket(Packet& p) const {
        if (multipart.isFile()) {
            File::Message::makeQS(p);
            p.addQS(tg_api::message_id, messageID);
            {
                p.beginQS(tg_api::media);
                p('{');
                p[tg_api::type] = multipart.getType();
                p[tg_api::media] = multipart.getAttachName();
                if (caption.length()) p[tg_api::caption].escape(caption);
                p('}');
            }
        } else {
            File::Message::makePacket(p);
            p[tg_api::message_id] = messageID;
            {
                p[tg_api::media]('{');
                p[tg_api::type] = multipart.getType();
                p[tg_api::media] = multipart.getUrlid();
                if (caption.length()) p[tg_api::caption].escape(caption);
                p('}');
            }
        }
    }
};
}
#endif