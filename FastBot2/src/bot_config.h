#pragma once
#define TELEGRAM_HOST "api.telegram.org"
#define TELEGRAM_PORT 443
#define FB_BLOCK_SIZE 256
#define FB_LONG_POLL_TOUT 300
#ifdef ESP8266
#define FB_ESP_YIELD() delay(0);
#else
#define FB_ESP_YIELD()
#endif
#if defined(ESP8266) || defined(ESP32)
#define FB_ESP_BUILD 1
#endif
#define FB_SUB_BEGIN [&]() {
#define FB_SUB_END }();
#define FB_SUB_NEXT FB_SUB_END FB_SUB_BEGIN
#ifdef FB_USE_LOG
#define FB_LOG(x)                                   \
    do {                                            \
        FB_USE_LOG.print(F("> "));                  \
        FB_USE_LOG.print(x);                        \
        FB_USE_LOG.print(F(" in "));                \
        FB_USE_LOG.print(__FUNCTION__);             \
        FB_USE_LOG.print(F("() [" __FILE__ " : ")); \
        FB_USE_LOG.print(__LINE__);                 \
        FB_USE_LOG.println(']');                    \
    } while (0);
#else
#define FB_LOG(x)
#endif