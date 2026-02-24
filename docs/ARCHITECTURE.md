# Правила и архитектура проекта WorldOfEmpires

Документ для использования при генерации кода через ИИ. Следуй этим правилам при создании или изменении файлов.

---

## 1. Архитектура приложения

### 1.1. Платформа

- **Движок:** Unreal Engine 5.7
- **Язык:** C++ (основная логика), Blueprint (опционально)
- **Модуль:** WorldOfEmpires (Runtime)

### 1.2. Разделение слоёв

| Слой | Путь | Назначение |
|------|------|------------|
| **Core** | `Source/WorldOfEmpires/Core/` | Игровая логика: GameMode, GameState, PlayerState, Actors |
| **Модуль** | `Source/WorldOfEmpires/` | Регистрация модуля, Build.cs |

### 1.3. Зависимости модуля (WorldOfEmpires.Build.cs)

- **Public:** Core, CoreUObject, Engine, InputCore, EnhancedInput, GameplayTags, UMG, NetCore
- **Private:** Slate, SlateCore

---

## 2. Структура каталогов

```
Source/
├── WorldOfEmpires/
│   ├── WorldOfEmpires.h          # Заголовок модуля
│   ├── WorldOfEmpires.cpp        # IMPLEMENT_PRIMARY_GAME_MODULE
│   ├── WorldOfEmpires.Build.cs   # Конфигурация сборки
│   └── Core/                     # Игровая логика
│       ├── WoE_GameMode.h/cpp
│       ├── WoE_GameState.h/cpp
│       ├── WoE_PlayerState.h/cpp
│       └── WoE_Character.h/cpp
├── WorldOfEmpires.Target.cs      # Таргет игры
└── WorldOfEmpiresEditor.Target.cs # Таргет редактора
```

---

## 3. Правила именования

### 3.1. Префиксы

| Тип | Префикс | Пример |
|-----|---------|--------|
| Actor | A | AWoE_GameMode, AWoE_Character |
| UObject | U | UWoE_InventoryComponent |
| Struct | F | FWoE_ItemData |
| Enum | E | EWoE_ItemType |
| Interface | I | IWoE_Interactable |

### 3.2. Имена файлов

- **Формат:** `WoE_<ИмяКласса>.h` / `WoE_<ИмяКласса>.cpp`
- **Стиль:** PascalCase с префиксом WoE_
- Примеры: `WoE_GameMode.h`, `WoE_PlayerState.cpp`

### 3.3. Макросы UE

- `UCLASS()` — для классов, наследуемых от UObject/AActor
- `UPROPERTY()` — для свойств (Replicated, BlueprintReadOnly, Category)
- `UFUNCTION()` — для функций, вызываемых из Blueprint или RPC
- `GENERATED_BODY()` — обязательно в UCLASS

---

## 4. Куда помещать файлы

| Тип кода | Куда |
|----------|------|
| GameMode, GameState, PlayerState | `Source/WorldOfEmpires/Core/` |
| Персонаж, Pawn | `Source/WorldOfEmpires/Core/` |
| Компоненты (ActorComponent) | `Source/WorldOfEmpires/Core/` или подпапка `Components/` |
| Типы, структуры, enum | В заголовке класса или отдельный `WoE_Types.h` |
| Константы | В заголовке или `WoE_Constants.h` |

---

## 5. Сетевая репликация

Для реплицируемых свойств:

1. Добавить `Replicated` в UPROPERTY
2. Переопределить `GetLifetimeReplicatedProps`
3. Зарегистрировать через `DOREPLIFETIME(ClassName, PropertyName)`
4. Include: `#include "Net/UnrealNetwork.h"`

---

## 6. Стиль кода (C++)

- Отступы: 4 пробела (или табы по настройкам проекта)
- Имена классов: PascalCase
- Имена переменных/функций: PascalCase (Unreal convention)
- Комментарии: на английском (с v0.2.1)

---

## 7. Добавление новой фичи

1. Создать `WoE_<Имя>.h` и `WoE_<Имя>.cpp` в `Core/` (или подпапке)
2. Добавить в Build.cs зависимости, если нужны новые модули
3. Зарегистрировать в GameMode (если это GameState/PlayerState/Pawn)
4. Обновить **AI_CONTEXT.md** — добавить новый класс/API

---

*Документ актуален для версии 0.2.2*

**Как обновлять:** при финализации (PROMPT_FINALIZE) ИИ автоматически проверяет структуру проекта и синхронизирует этот документ. Ручное обновление — см. DEVELOPER_GUIDE.md
