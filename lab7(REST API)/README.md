# Лабораторная работа №7 — Цитаты Стетхема

REST-клиент на C++/Qt, который парсит цитаты с сайта citaty.info и отображает случайную цитату.

## Требования

- Qt 6.x (Core, Gui, Widgets, Network)
- CMake 3.16+
- C++17

## Сборка

```bash
mkdir build && cd build
cmake ..
cmake --build .
```

## Запуск

```bash
./QuoteApp          # Linux / macOS
QuoteApp.exe        # Windows
```

## Структура проекта

```
qt_quotes/
├── CMakeLists.txt
├── README.md
└── src/
    ├── main.cpp            # точка входа
    ├── MainWindow.h/.cpp   # главное окно, GUI
    └── QuoteFetcher.h/.cpp # HTTP-запрос + парсинг HTML
```

## Как работает

1. Пользователь нажимает кнопку «Получить цитату».
2. `QuoteFetcher` отправляет GET-запрос на `https://citaty.info/selection/citaty-stethema`.
3. Из HTML-ответа извлекаются все `<blockquote>` блоки с помощью регулярных выражений.
4. Случайная цитата отображается в интерфейсе (текст, автор, теги).
5. Все сетевые и программные ошибки обрабатываются и выводятся в интерфейсе.
