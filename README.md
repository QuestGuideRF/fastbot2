# FastBot2

Библиотека для создания Telegram ботов на Arduino, ESP8266 и ESP32. Предоставляет быстрый и эффективный способ взаимодействия с Telegram Bot API с минимальным использованием памяти.

## ✨ Особенности

- **Несколько режимов опроса**, включая long polling для мгновенной доставки сообщений
- **Быстрый парсинг ответов** сервера (в 6 раз быстрее библиотек на базе ArduinoJSON)
- **Удобные классы-обёртки** для всех основных операций Telegram Bot API
- **Полный доступ к API** в "ручном режиме" для реализации любых функций
- **Лёгкий вес** и эффективное использование оперативной памяти
- **Нативная поддержка WiFi** для ESP8266/ESP32
- **Работа с файлами**: загрузка, скачивание, OTA обновление (ESP8266/ESP32)
- **Работа через Arduino Client**, поддержка Ethernet, GSM и других интерфейсов

## 🚀 Быстрый старт

### Установка

Библиотеку можно установить через менеджер библиотек в:
- Arduino IDE
- Arduino IDE v2
- PlatformIO (поиск по названию **FastBot2**)

Или скачать вручную:
1. Скачайте [архив библиотеки](https://github.com/QuestGuideRF/fastbot2/archive/refs/heads/main.zip)
2. Распакуйте и поместите в папку `libraries` вашей Arduino IDE
3. Перезапустите Arduino IDE

### Зависимости

- [GSON](https://github.com/GyverLibs/GSON) v1.8.0+
- [StringUtils](https://github.com/GyverLibs/StringUtils) v1.5.0+
- [GyverHTTP](https://github.com/GyverLibs/GyverHTTP) v1.0.27+
- [GTL](https://github.com/GyverLibs/GTL) v1.3.1+

### Настройка бота

1. Создайте бота через [@BotFather](https://t.me/BotFather):
   - Отправьте команду `/newbot`
   - Укажите имя и username бота (должен оканчиваться на `bot`)
   - Сохраните полученный **токен**

2. Настройки в BotFather:
   - **Webhook** должен быть отключен (по умолчанию отключен)
   - Для работы в группах отключите **Group Privacy** в настройках бота
   - В группах бота нужно сделать администратором

### Минимальный пример (ESP8266/ESP32)

```cpp
#define WIFI_SSID "your_wifi_ssid"
#define WIFI_PASS "your_wifi_password"
#define BOT_TOKEN "your_bot_token"

#include <FastBot2.h>
FastBot2 bot;

void update(fb::Update& u) {
    // Вывести информацию о сообщении
    Serial.println(u.message().text());

    // Ответить пользователю
    bot.sendMessage(fb::Message(
        u.message().text(),
        u.message().chat().id()
    ));
}

void setup() {
    Serial.begin(115200);

    // Подключение к WiFi
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("Connected");

    // Настройка бота
    bot.setToken(BOT_TOKEN);
    bot.onUpdate(update);
    bot.setPollMode(fb::Poll::Long, 60000);  // Long polling
}

void loop() {
    bot.tick();  // Обработка обновлений
}
```

### Другие платформы

Для платформ, отличных от ESP8266/ESP32, используйте класс `FastBot2Client` с любым клиентом, поддерживающим интерфейс `Arduino Client`:

```cpp
SomeClient client;  // Ethernet, GSM и т.д.

#include <FastBot2Client.h>
FastBot2Client bot(client);

void setup() {
    bot.setToken("your_token");
    bot.onUpdate(update);
}
```

## 📁 Структура проекта

```
FastBot2-main/
├── src/                    # Исходный код библиотеки
│   ├── core/               # Ядро библиотеки
│   │   ├── api.h/cpp       # Работа с Telegram API
│   │   ├── core_class.h/cpp # Основной класс
│   │   └── types/          # Типы данных для работы с API
│   ├── FastBot2.h          # Класс для ESP8266/ESP32
│   └── FastBot2Client.h   # Базовый класс для других платформ
├── examples/               # Примеры использования
│   ├── minimal/            # Минимальный пример
│   ├── inlineMenu/         # Работа с инлайн-меню
│   ├── esp32cam-send/      # Отправка фото с камеры
│   └── test/               # Тестовые примеры
└── README.md               # Документация
```

## 🛠 Использование

### Настройка бота

```cpp
void setup() {
    // ОБЯЗАТЕЛЬНО: установка токена
    bot.setToken("your_token");

    // НЕОБЯЗАТЕЛЬНО (значения по умолчанию):
    bot.skipUpdates(-1);              // Пропустить все старые сообщения
    bot.setLimit(3);                  // Максимум сообщений в одном обновлении
    bot.setPollMode(fb::Poll::Sync, 4000);  // Режим опроса
    bot.setTimeout(2000);              // Таймаут ожидания ответа (мс)
    bot.setMemLimit(20000);            // Лимит памяти на ответ (байт)
}
```

### Режимы опроса

#### Sync (по умолчанию)
```cpp
bot.setPollMode(fb::Poll::Sync, 4000);
```
- Синхронный режим: библиотека ждёт ответ внутри `tick()`
- Рекомендуемый период: > 3500 мс
- Отправка сообщений быстрая в любом месте программы

#### Async
```cpp
bot.setPollMode(fb::Poll::Async, 4000);
```
- Асинхронный режим: запрос отправляется, ответ обрабатывается позже
- Рекомендуемый период: > 3500 мс
- Для безопасной отправки проверяйте `isPolling()`

#### Long (рекомендуется)
```cpp
bot.setPollMode(fb::Poll::Long, 60000);
```
- Long polling: ответ приходит либо по таймауту, либо при новых сообщениях
- Рекомендуемый период: > 20000 мс
- Самый быстрый способ получения обновлений
- Сообщения доставляются мгновенно

### Обработка обновлений

```cpp
void update(fb::Update& u) {
    // Проверка типа обновления
    if (u.isMessage()) {
        // Работа с сообщением
        Serial.println(u.message().text());
        Serial.println(u.message().from().username());
    }

    if (u.isQuery()) {
        // Обработка callback query (нажатие на кнопку)
        Serial.println(u.query().data());
        bot.answerCallbackQuery(u.query().id(), "Обработано!");
    }
}

void setup() {
    bot.onUpdate(update);
}
```

### Отправка сообщений

```cpp
// Простое сообщение
fb::Message msg("Привет!", chatID);
bot.sendMessage(msg);

// С форматированием HTML
fb::Message msg;
msg.text = "<b>Жирный</b> и <i>курсивный</i> текст";
msg.mode = fb::Message::Mode::HTML;
msg.chatID = chatID;
bot.sendMessage(msg);

// С инлайн-меню
fb::Message msg("Выберите действие:", chatID);
fb::InlineMenu menu;
menu.addButton("Кнопка 1", "action1");
menu.addButton("Кнопка 2", "action2");
msg.setInlineMenu(menu);
bot.sendMessage(msg);
```

### Работа с файлами

#### Отправка файла
```cpp
// Из файловой системы (ESP)
File file = LittleFS.open("/image.jpg", "r");
fb::File f("photo.jpg", fb::File::Type::photo, file);
f.chatID = chatID;
f.caption = "Фото с камеры";
bot.sendFile(f);

// Из буфера в памяти
uint8_t buffer[1024];
// ... заполнение буфера ...
fb::File f("data.bin", fb::File::Type::document, buffer, 1024);
f.chatID = chatID;
bot.sendFile(f);
```

#### Скачивание файла
```cpp
void update(fb::Update& u) {
    if (u.message().hasDocument()) {
        fb::Fetcher fetch = bot.downloadFile(u.message().document().id());

        if (fetch) {
            // Записать в файл
            File file = LittleFS.open("downloaded.bin", "w");
            fetch.writeTo(file);
            file.close();
        }
    }
}
```

#### OTA обновление (ESP8266/ESP32)
```cpp
void update(fb::Update& u) {
    if (u.message().hasDocument() &&
        u.message().document().name().endsWith(".bin")) {
        // Обновление прошивки
        bot.updateFlash(u.message().document(), u.message().chat().id());
    }
}
```

### Работа с меню

#### Обычная клавиатура
```cpp
fb::Message msg("Выберите:", chatID);
fb::Menu menu;
menu.addButton("Кнопка 1").addButton("Кнопка 2").newRow();
menu.addButton("Кнопка 3");
msg.setMenu(menu);
bot.sendMessage(msg);
```

#### Инлайн-меню
```cpp
fb::Message msg("Меню:", chatID);
fb::InlineMenu menu;
menu.addButton("Действие 1", "action1");
menu.addButton("Ссылка", "https://example.com");
menu.newRow();
menu.addButton("Действие 2", "action2");
msg.setInlineMenu(menu);
bot.sendMessage(msg);

// Обработка нажатий
void update(fb::Update& u) {
    if (u.isQuery()) {
        switch (u.query().data().hash()) {
            case SH("action1"):
                // Обработка действия 1
                break;
            case SH("action2"):
                // Обработка действия 2
                break;
        }
        bot.answerCallbackQuery(u.query().id());
    }
}
```

### Редактирование сообщений

```cpp
// Редактирование текста
fb::TextEdit edit;
edit.text = "Новый текст";
edit.chatID = chatID;
edit.messageID = messageID;
bot.editText(edit);

// Редактирование меню
fb::MenuEdit edit;
edit.chatID = chatID;
edit.messageID = messageID;
fb::InlineMenu newMenu;
newMenu.addButton("Новая кнопка", "new_action");
edit.setInlineMenu(newMenu);
bot.editMenu(edit);
```

### Ручная работа с API

Для доступа ко всем возможностям Telegram Bot API:

```cpp
// Отправка команды вручную
fb::Packet p = bot.beginPacket(tg_cmd::sendMessage);
p[tg_api::chat_id] = chatID;
p[tg_api::text] = "Сообщение";
bot.sendPacket(p);

// Или через sendCommand
bot.sendCommand(tg_cmd::getMe);
```

### Фильтрация по пользователям

```cpp
void update(fb::Update& u) {
    // Только для одного админа
    if (u.message().from().id() != 12345678LL) return;

    // Для нескольких админов
    int64_t admin_ids[] = {12345678, 87654321};
    int64_t user_id = u.message().from().id();
    bool is_admin = false;
    for (int i = 0; i < 2; i++) {
        if (admin_ids[i] == user_id) {
            is_admin = true;
            break;
        }
    }
    if (!is_admin) return;

    // Ваш код для админов
}
```

## 🧪 Технологии

- **Arduino Framework** - базовая платформа
- **GSON** - быстрый парсинг JSON
- **StringUtils** - работа со строками
- **GyverHTTP** - HTTP клиент
- **GTL** - вспомогательные утилиты

## ⚙️ Настройки компиляции

Можно настроить перед подключением библиотеки:

```cpp
#define FB_NO_FILE          // Убрать поддержку работы с файлами
#define FB_USE_LOG Serial   // Выводить логи ошибок

#include <FastBot2.h>
```

## 📝 Лимиты Telegram

### Отправка
- В личный чат: не чаще 1 раза в секунду
- В группу: не чаще 20 сообщений в минуту
- Суммарно: не чаще 30 сообщений в секунду

### Чтение
- Бот может работать с сообщениями, отправленными не более 24 часов назад
- Бот не видит сообщения от других ботов в группе
- Длинные сообщения (>4000 символов) разбиваются на несколько

## 🔧 Основные классы и методы

### FastBot2 / FastBot2Client

Основные методы:
- `setToken(token)` - установка токена
- `onUpdate(callback)` - подключение обработчика обновлений
- `tick()` - обработка обновлений (вызывать в loop)
- `sendMessage(msg)` - отправка сообщения
- `sendFile(file)` - отправка файла
- `editText(edit)` - редактирование текста
- `deleteMessage(chatID, messageID)` - удаление сообщения

### fb::Update

Методы для работы с обновлениями:
- `type()` - тип обновления
- `isMessage()` - проверка на сообщение
- `isQuery()` - проверка на callback query
- `message()` - доступ к сообщению
- `query()` - доступ к query

### fb::MessageRead

Чтение данных сообщения:
- `text()` - текст сообщения
- `from()` - отправитель
- `chat()` - чат
- `id()` - ID сообщения
- `hasDocument()` - проверка наличия документа
- `hasLocation()` - проверка наличия геолокации

### fb::Message

Создание сообщения для отправки:
- `text` - текст сообщения
- `chatID` - ID чата
- `mode` - режим форматирования (Text, HTML, MarkdownV2)
- `setMenu(menu)` - добавить клавиатуру
- `setInlineMenu(menu)` - добавить инлайн-меню

Полный список всех классов и методов смотрите в примерах и исходном коде библиотеки.

## 📚 Примеры

В папке `examples` находятся готовые примеры:
- **minimal** - минимальный рабочий пример
- **inlineMenu** - работа с инлайн-меню
- **esp32cam-send** - отправка фото с камеры ESP32-CAM
- **test** - различные тестовые сценарии

## 🔍 Дополнительная информация

### Указатель на текущий бот

Внутри обработчика обновлений доступен указатель на текущий объект бота:

```cpp
void update(fb::Update& u) {
    static_cast<FastBot2*>(fb::thisBot)->sendMessage(...);
}
```

### Тип данных Text

Все значения из обновлений представлены типом `Text`:
- Печать в любой `Print` (Serial, и т.д.)
- Сравнение со строками
- Конвертация в числа
- Декодирование Unicode

```cpp
u.message().text() == "команда";  // Сравнение
int64_t id = u.message().from().id();  // Конвертация
Serial.println(u.message().text());  // Печать
```

### Оптимизация памяти

Для уменьшения использования памяти оборачивайте запросы в блоки:

```cpp
{
    fb::Result res = bot.sendCommand(tg_cmd::getMe);
    Serial.println(res[tg_apih::first_name]);
}  // Память освобождается здесь
```

Или вызывайте `reset()`:

```cpp
fb::Result res = bot.sendCommand(tg_cmd::getMe);
// ... работа с результатом ...
res.reset();  // Освобождение памяти
```

Подпись относится к текущей версии проекта.

---

Автор: legenda  
Telegram: [@LEGENDA_SD](https://t.me/LEGENDA_SD)
