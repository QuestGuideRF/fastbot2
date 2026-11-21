#include <Arduino.h>
#define WIFI_SSID ""
#define WIFI_PASS ""
#define BOT_TOKEN ""
#define CHAT_ID ""
#include <FastBot2.h>
FastBot2 bot;
void updateh(fb::Update& u) {
    if (u.isQuery()) {
        Serial.println("NEW QUERY");
        Serial.println(u.query().data());
        bot.answerCallbackQuery(u.query().id(), "query answered");
        switch (u.query().data().hash()) {
            case "test"_h:
                Serial.println("включить светодиод");
                break;
            case "pest"_h:
                break;
            case "lol"_h:
                break;
        }
    }
}
void setup() {
    Serial.begin(115200);
    Serial.println();
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("Connected");
    bot.attachUpdate(updateh);
    bot.setToken(F(BOT_TOKEN));
    bot.setPollMode(fb::Poll::Long, 60000);
    fb::Message msg("Send inline menu", CHAT_ID);
    fb::InlineMenu menu("kek 1 ; kek 2 ; kek 3 \n kek 4 ; kek 5", "test;pest;lol;https://www.google.ru/;https://www.yandex.ru/");
    msg.setInlineMenu(menu);
    bot.sendMessage(msg);
}
void loop() {
    bot.tick();
}