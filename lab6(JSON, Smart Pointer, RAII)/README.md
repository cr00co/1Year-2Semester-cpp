# Lab 6 — JSON, Smart Pointer, RAII (Вариант 2)

## Что делает программа

1. Нажимаем **Parse JSON** → выбираем `warehouse.json`
2. Программа фильтрует просроченные товары (оставляет только свежие)
3. В названии каждого товара все гласные буквы заменяются на ВЕРХНИЙ регистр
4. Результат выводится в GUI и сохраняется в `variant.json`

## Архитектура

| Файл | Роль |
|---|---|
| `Logger.hpp` | Логгер из лаб. 3 |
| `Product.hpp` | Структура товара |
| `ProductFactory.hpp` | **Фабрика** — создаёт `Product` из JSON |
| `Processor.hpp` | Бизнес-логика варианта 2 |
| `FileGuard.hpp` | **RAII #1** — управление файлом |
| `TimerGuard.hpp` | **RAII #2** — замер времени |
| `Facade.hpp` | **Фасад** — единая точка запуска |
| `MainWindow.hpp` | Qt GUI |
| `main.cpp` | Точка входа |

## Зависимости

- Qt 6 (или Qt 5 — поменяй в .pro `QT += core widgets`)
- [nlohmann/json](https://github.com/nlohmann/json) — скачай `json.hpp` и положи в `src/nlohmann/json.hpp`

## Сборка через CMake

```bash
cmake -B build
cmake --build build
./build/Lab6
```

## Формат warehouse.json

```json
[
  {
    "id": 1,
    "name": "Apple",
    "category": "Fruit",
    "price": 1.5,
    "value": 100,
    "expiration_date": "2026-12-01"
  }
]
```

## Требования ТЗ

- ✅ Умные указатели (`unique_ptr`, `shared_ptr`) — сырые указатели не используются
- ✅ RAII #1 — `FileGuard` (открытие/закрытие файла)
- ✅ RAII #2 — `TimerGuard` (замер времени)
- ✅ Паттерн Фабрика — `ProductFactory`
- ✅ Паттерн Фасад — `Facade`
- ✅ nlohmann/json для работы с JSON
- ✅ Logger из 3-й лабораторной
- ✅ GUI: кнопка + область вывода
