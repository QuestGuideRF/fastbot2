#include <Arduino.h>
#define WIFI_SSID ""
#define WIFI_PASS ""
#define BOT_TOKEN ""
#define CHAT_ID ""
#include <FastBot2.h>
FastBot2 bot;
void updateh(fb::Update& u) {
    Serial.println("NEW MESSAGE");
    Serial.println(u.message().from().username());
    Serial.println(u.message().text());
    bot.sendMessage(fb::Message(u.message().text(), u.message().chat().id()));
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
    bot.sendMessage(fb::Message("Hello!", CHAT_ID));
}
void loop() {
    bot.tick();
}