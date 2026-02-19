# Контекст для ИИ — WorldOfEmpires

Документ описывает **уже реализованный код** проекта. Используй его вместе с **ARCHITECTURE.md** при генерации нового функционала.

**Перед генерацией кода:**
1. Прочитай **ARCHITECTURE.md** — правила, структура, именование.
2. Используй этот документ — что уже есть, какие классы и API доступны.
3. Не дублируй существующий код. Используй готовые классы и расширяй их.

---

## 1. Связь документов

| Документ | Назначение |
|----------|------------|
| **ARCHITECTURE.md** | Как устроен проект, куда класть файлы, правила именования |
| **AI_CONTEXT.md** (этот) | Что уже реализовано, какие классы, свойства, как их использовать |

---

## 2. Модуль WorldOfEmpires — Core

### 2.1. AWoE_GameMode

**Файлы:** `Source/WorldOfEmpires/Core/WoE_GameMode.h`, `WoE_GameMode.cpp`

| Свойство/метод | Описание |
|----------------|----------|
| `GameStateClass` | `AWoE_GameState::StaticClass()` |
| `PlayerStateClass` | `AWoE_PlayerState::StaticClass()` |
| `BeginPlay()` | Логирует «WoEGameMode: BeginPlay вызван. Сервер запущен.» |

**Использование:** Назначить в World Settings → GameMode Override.

---

### 2.2. AWoE_GameState

**Файлы:** `Source/WorldOfEmpires/Core/WoE_GameState.h`, `WoE_GameState.cpp`

| Свойство | Тип | Replicated | Описание |
|----------|-----|------------|----------|
| `GameTimeOfDay` | `float` | ✅ | Время суток (0–24). Начальное: 8.0 |

**Методы:**
- `GetLifetimeReplicatedProps` — регистрирует `GameTimeOfDay` для репликации

**Использование:** Получить через `GetWorld()->GetGameState<AWoE_GameState>()` или в Blueprint.

---

### 2.3. AWoE_PlayerState

**Файлы:** `Source/WorldOfEmpires/Core/WoE_PlayerState.h`, `WoE_PlayerState.cpp`

| Свойство | Тип | Replicated | Описание |
|----------|-----|------------|----------|
| `ClanId` | `int32` | ✅ | ID клана. 0 = нет клана |
| `DisplayName` | `FString` | ✅ | Отображаемое имя. По умолчанию «Survivor» |

**Методы:**
- `GetLifetimeReplicatedProps` — регистрирует `ClanId`, `DisplayName`

**Использование:** `GetPlayerState<AWoE_PlayerState>()` на сервере или клиенте.

---

## 3. Зависимости (WorldOfEmpires.Build.cs)

| Модуль | Назначение |
|--------|------------|
| Core, CoreUObject, Engine | Базовые типы UE |
| InputCore, EnhancedInput | Ввод (пока не используется) |
| GameplayTags | Теги (пока не используется) |
| UMG | UI виджеты (пока не используется) |
| NetCore | Сетевая репликация |
| Slate, SlateCore | Низкоуровневый UI |

---

## 4. Плагины

| Плагин | Назначение |
|--------|------------|
| ModelingToolsEditorMode | Редактор (включён для Editor) |

---

## 5. Типичный сценарий генерации

### Добавить персонажа (AWoECharacter)

1. Создать `WoE_Character.h/cpp` в `Core/`
2. Наследовать от `ACharacter`
3. В `AWoE_GameMode` установить `DefaultPawnClass = AWoE_Character::StaticClass()`
4. Добавить в AI_CONTEXT.md

### Добавить реплицируемое свойство

1. Добавить `UPROPERTY(Replicated, BlueprintReadOnly, Category="...")` в заголовок
2. В `.cpp`: `#include "Net/UnrealNetwork.h"`
3. В `GetLifetimeReplicatedProps`: `DOREPLIFETIME(ClassName, PropertyName)`

### Добавить UI (HUD)

1. Создать Widget Blueprint или C++ виджет (UMG)
2. Подключить в PlayerController или HUD
3. Использовать `GetGameState<AWoE_GameState>()->GameTimeOfDay` и т.п.

---

## 6. Промпт для ИИ (шаблон)

```
Проект: WorldOfEmpires (Unreal Engine 5.7).
Архитектура: ARCHITECTURE.md
Реализованный код: AI_CONTEXT.md

Задача: [описание]

Учитывай:
- Существующие классы AWoE_GameMode, AWoE_GameState, AWoE_PlayerState
- Префикс WoE_ для новых классов
- Следуй структуре из ARCHITECTURE.md
```

---

*Документ актуален для версии 0.1.1. Обновляй при добавлении нового API.*

**Как обновлять:** см. DEVELOPER_GUIDE.md (раздел «Как правильно зафиксировать изменения»)
