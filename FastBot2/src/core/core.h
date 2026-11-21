#pragma once
#include <Arduino.h>
#include <GSON.h>
#include <StringUtils.h>
#ifndef __AVR__
#include <functional>
#endif
#include "../bot_config.h"
#include "Fetcher.h"
#include "api.h"
#include "core_class.h"
#include "http.h"
#include "packet.h"
#include "result.h"
#include "types/Update.h"
#include "updates.h"
namespace fb {
enum class Poll : uint8_t {
    Sync,
    Async,
    Long,
};
class Core : public Http {
   protected:
#ifdef __AVR__
    typedef void (*CallbackError)(Text text);
    typedef void (*CallbackResultOld)(gson::Entry& entry);
    typedef void (*CallbackResult)(fb::Result& res);
    typedef void (*CallbackRaw)(Text response);
    typedef void (*CallbackUpdate)(Update& upd);
#else
    typedef std::function<void(Text text)> CallbackError;
    typedef std::function<void(gson::Entry& entry)> CallbackResultOld;
    typedef std::function<void(fb::Result& res)> CallbackResult;
    typedef std::function<void(Text response)> CallbackRaw;
    typedef std::function<void(Update& upd)> CallbackUpdate;
#endif
    enum class ota_t : uint8_t {
        None,
        Flash,
        FS,
    };
   public:
    Updates updates;
    Core(Client& client, const String& token = "") : Http(client) {
        _token.reserve(47);
        _token = token;
    }
    void setTimeout(uint16_t timeout) {
        _clientTout = timeout;
        http.setTimeout(timeout);
    }
    uint16_t getTimeout() {
        return _clientTout;
    }
    void decodeUCN(bool decode) {
        _dec_ucn = decode;
    }
    void setToken(const String& token) {
        _token = token;
    }
    const String& getToken() {
        return _token;
    }
    void setMemLimit(size_t limit) {
        _limit = limit;
    }
    size_t getMemLimit(void) {
        return _limit;
    }
    void setLimit(uint8_t limit = 3) {
        _poll_limit = limit ? limit : 1;
    }
    uint8_t getLimit(void) {
        return _limit;
    }
    void setPollMode(Poll mode = Poll::Sync, uint16_t period = 4000) {
        _poll_mode = mode;
        _poll_prd = period;
    }
    Poll getPollMode() {
        return _poll_mode;
    }
    uint16_t getPollPeriod() {
        return _poll_prd;
    }
    void skipUpdates(int32_t offset = -1) {
        _poll_offset = offset;
    }
    void skipNextMessage() {
        _poll_offset++;
    }
    void autoIncrement(bool incr) {
        _incr_auto = incr;
    }
    void autoQuery(bool query) {
        _query_auto = query;
    }
    void exitUpdates() {
        _exit_f = true;
    }
    uint32_t lastBotMessage() {
        return _last_bot;
    }
    void begin() {
        _state = true;
    }
    void end() {
        _state = false;
        http.stop();
    }
    void setOnline(bool online) {
        _online = online;
    }
    void attachUpdate(CallbackUpdate callback) {
        _cbUpdate = callback;
    }
    void onUpdate(CallbackUpdate callback) {
        _cbUpdate = callback;
    }
    void detachUpdate() {
        _cbUpdate = nullptr;
    }
    void attachRaw(CallbackRaw callback) {
        _cbRaw = callback;
    }
    void onRaw(CallbackRaw callback) {
        _cbRaw = callback;
    }
    void detachRaw() {
        _cbRaw = nullptr;
    }
    void onResult(CallbackResult callback) {
        _cbRes = callback;
    }
    void detachResult() {
        _cbRes = nullptr;
        _cbResultOld = nullptr;
    }
    void attachResult(CallbackResultOld callback) {
        _cbResultOld = callback;
    }
    void onResult(CallbackResultOld callback) {
        _cbResultOld = callback;
    }
    void attachError(CallbackError callback) {
        _cbErr = callback;
    }
    void onError(CallbackError callback) {
        _cbErr = callback;
    }
    void detachError() {
        _cbErr = nullptr;
    }
    bool tick() {
        if (!_state) return 0;
#if !defined(FB_NO_FILE) && (defined(ESP8266) || defined(ESP32))
        if (_ota != ota_t::None) {
            if (_ota_id.length()) {
                fb::Fetcher fetch = downloadFile(_ota_id);
                if (fetch) {
                    bool ok = false;
                    ok = (_ota == ota_t::Flash) ? fetch.updateFlash() : fetch.updateFS();
                    _sendMessage(ok ? F("ОТА ОК") : F("Ошибка ОТА"), _ota_user);
                    if (ok) _reboot = Fetcher::Reboot::WaitUpdate;
                } else {
                    _sendMessage(F("Ошибка получения OTA"), _ota_user);
                }
            }
            _ota = ota_t::None;
        }
#endif
        if (http.isWaiting()) {
            if (millis() - _last_send >= (_clientTout + (_poll_wait ? _poll_prd : 0ul))) {
                _poll_wait = 0;
                FB_LOG("тайм-аут опроса");
                http.stop();
            }
        } else if (_online) {
            bool tout = millis() - _last_send >= (_poll_mode == Poll::Long ? FB_LONG_POLL_TOUT : _poll_prd);
            if (!_last_send || tout) {
                if (_poll_mode == Poll::Sync) {
                    Result res = getUpdates(true);
                    if (res.isArray()) {
                        _parseUpdates(res);
                        return 1;
                    }
                } else {
                    getUpdates(false);
                }
            }
        }
        if (http.available()) {
            _poll_wait = 0;
            Result res = _parseResponse(http.getResponse());
            if (res && res.isArray()) {
                _parseUpdates(res);
                return 1;
            }
        }
        return 0;
    }
    bool tickManual() {
        Result res = getUpdates(true, false);
        if (res && res.isArray()) {
            _parseUpdates(res);
            return 1;
        }
        return 0;
    }
    bool isPolling() {
        return _poll_wait;
    }
    Result getUpdates(bool wait, bool allowLongPool = true) {
        if (!_token.length()) return Result();
        FB_LOG("getUpdates");
        Packet p(tg_cmd::getUpdates, _token);
        if (_poll_mode == Poll::Long && allowLongPool) p[tg_api::timeout] = (uint16_t)(_poll_prd / 1000);
        p[tg_api::limit] = _poll_limit;
        p[tg_api::offset] = _poll_offset;
        updates.makePacket(p);
        if (wait) {
            _poll_wait = 0;
            return sendPacket(p, true);
        } else {
            return sendPacket(p, false, &_poll_wait);
        }
    }
    void reboot() {
        _reboot = Fetcher::Reboot::Triggered;
    }
    bool canReboot() {
        return _reboot == Fetcher::Reboot::CanReboot;
    }
    Result sendCommand(const __FlashStringHelper* cmd, const Text& json, bool wait = true) {
        Packet p(cmd, _token, json);
        return sendPacket(p, wait);
    }
    Result sendCommand(const __FlashStringHelper* cmd, const gson::Str& json, bool wait = true) {
        Packet p(cmd, _token, json);
        return sendPacket(p, wait);
    }
    Result sendCommand(const __FlashStringHelper* cmd, bool wait = true) {
        Packet p(cmd, _token, "");
        return sendPacket(p, wait);
    }
    Packet beginPacket(const __FlashStringHelper* cmd) {
        return Packet(cmd, _token);
    }
    Result sendPacket(Packet& packet, bool wait = true, bool* sent = nullptr) {
        if (!_token.length()) return Result();
        if (_poll_wait) {
            _poll_wait = 0;
            FB_LOG("прекратить опрос");
            http.stop();
        }
        FB_ESP_YIELD();
        if (http.beginSend()) {
            if (sent) *sent = true;
            packet.printTo(http);
            _last_send = millis();
            FB_ESP_YIELD();
            if (wait) {
                FB_LOG("отправить + подождать");
                return _parseResponse(http.getResponse());
            } else {
                FB_LOG("отправить асинхронный");
            }
        } else {
            if (sent) *sent = false;
            FB_LOG("ошибка отправки");
        }
        _last_send = millis();
        return Result();
    }
    String getFilePath(Text fileID) {
        if (!_token.length()) return String();
        FB_LOG("getFile");
        fb::Packet p(tg_cmd::getFile, _token);
        p[tg_api::file_id] = fileID;
        fb::Result res = sendPacket(p, true);
        FB_ESP_YIELD();
        if (res.has(tg_apih::file_id) && res[tg_apih::file_id] == fileID) {
            return res[tg_apih::file_path].toString();
        }
        return String();
    }
    String getFileLink(Text fileID) {
        if (!_token.length()) return String();
        String link = F("https://" TELEGRAM_HOST "/file/bot");
        link += _token;
        link += '/';
        link += getFilePath(fileID);
        return link;
    }
#if !defined(FB_NO_FILE) && (defined(ESP8266) || defined(ESP32))
    fb::Fetcher downloadFile(Text fileID) {
        FB_ESP_YIELD();
        StreamReader reader;
        String path = getFilePath(fileID);
        if (path.length()) {
            FB_LOG("скачать файл");
            fb::Packet p(path, _token);
            reader = sendPacket(p, true).getReader();
            FB_ESP_YIELD();
        }
        return fb::Fetcher(&_reboot, reader);
    }
#endif
   protected:
    String _token;
    uint16_t _clientTout = 3000;
    uint16_t _poll_prd = 4000;
    Poll _poll_mode = Poll::Sync;
    uint8_t _poll_limit = 3;
    bool _poll_wait = 0;
    bool _query_answ = 0;
    bool _query_auto = true;
    bool _state = true;
    bool _online = true;
    bool _incr_auto = true;
    bool _exit_f = false;
    bool _dec_ucn = true;
    int32_t _poll_offset = 0;
    uint32_t _last_bot = 0;
    uint32_t _last_send = 0;
    size_t _limit = 20000;
    Fetcher::Reboot _reboot = Fetcher::Reboot::Idle;
    CallbackRaw _cbRaw = nullptr;
    CallbackUpdate _cbUpdate = nullptr;
    CallbackResult _cbRes = nullptr;
    CallbackResultOld _cbResultOld = nullptr;
    CallbackError _cbErr = nullptr;
#if !defined(FB_NO_FILE) && (defined(ESP8266) || defined(ESP32))
    ota_t _ota = ota_t::None;
    String _ota_id;
    String _ota_user;
#endif
    Result _parseResponse(ghttp::Client::Response resp) {
        FB_ESP_YIELD();
        if (resp) {
            if (resp.type() == F("application/json")) {
                FB_LOG("получил JSON");
                Result res(resp.body());
                res.parseJson();
                http.flush();
                FB_ESP_YIELD();
                thisBot = this;
                if (_cbRaw) _cbRaw(res.getRaw());
                if (res) {
                    if (_dec_ucn) {
                        for (uint16_t i = 0; i < res._parser.length(); i++) {
                            switch (res._parser._keyHash(i)) {
                                case tg_apih::bio:
                                case tg_apih::text:
                                case tg_apih::quote:
                                case tg_apih::title:
                                case tg_apih::caption:
                                case tg_apih::file_name:
                                case tg_apih::last_name:
                                case tg_apih::first_name:
                                case tg_apih::description:
                                    res._parser._getByIndex(i).decodeUCN();
                                    break;
                            }
                        }
                    }
                    if (res.isObject()) {
                        if (res.has(tg_apih::message_id)) _last_bot = res[tg_apih::message_id];
                        if (_cbResultOld) _cbResultOld(res);
                    }
                } else {
                    if (_cbErr && res.isError()) _cbErr(res.getError());
                }
                if (_cbRes && !res.isArray()) _cbRes(res);
                thisBot = nullptr;
                return res;
            } else if (resp.type() == F("application/octet-stream")) {
                FB_LOG("получил файл");
                return Result(resp.body());
            } else {
                FB_LOG("неизвестный ответ");
                http.flush();
            }
        } else {
            FB_LOG("плохой ответ");
        }
        return Result();
    }
    void _parseUpdates(gson::Entry& result) {
        if (_reboot == Fetcher::Reboot::WaitUpdate) {
            _reboot = Fetcher::Reboot::CanReboot;
#ifdef FB_ESP_BUILD
            ESP.restart();
#endif
            return;
        }
        updates.changed = false;
        thisBot = this;
        _exit_f = false;
        uint8_t len = result.length();
        for (uint8_t i = 0; i < len; i++) {
            FB_ESP_YIELD();
            gson::Entry upd = result[i][1];
            if (!upd || !upd.isObject()) continue;
            uint32_t offset = result[i][tg_apih::update_id].toInt32();
            if (!_poll_offset) _poll_offset = offset;
            if (_incr_auto) _poll_offset = offset + 1;
            size_t typeHash = upd.keyHash();
            Update update(upd, typeHash, offset);
            if (typeHash == tg_apih::callback_query) _query_answ = false;
            if (_cbUpdate) _cbUpdate(update);
            FB_ESP_YIELD();
            if (typeHash == tg_apih::callback_query && !_query_answ) {
                _query_answ = true;
                if (_query_auto) {
                    fb::Packet p(tg_cmd::answerCallbackQuery, _token);
                    p[tg_api::callback_query_id] = update.query().id();
                    sendPacket(p, false);
                }
            }
            if (_reboot == Fetcher::Reboot::Triggered) {
                _reboot = Fetcher::Reboot::WaitUpdate;
                break;
            }
            if (_exit_f) break;
        }
        thisBot = nullptr;
    }
    void _sendMessage(Text text, const String& id) {
        if (!id.length() || !text.length()) return;
        fb::Packet p(tg_cmd::sendMessage, _token);
        p[tg_api::chat_id] = id;
        p[tg_api::text] = text;
        sendPacket(p, true);
    }
};
}