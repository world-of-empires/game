# WorldOfEmpires — Roadmap

Дорожная карта разработки игры на Unreal Engine 5.7.  
Отмеченные пункты считаются завершёнными на текущем этапе.

**Текущая версия:** v0.2.1 | **Последнее обновление:** 19.02.2026

---

## Блок 0: Подготовка проекта — v0.2.0

### Часть 1–2: Создание проекта и структура папок

- [x] Создание проекта Unreal Engine 5.7
- [x] Настройка модуля WorldOfEmpires (WorldOfEmpires.Build.cs)
- [x] Настройка таргетов Game и Editor
- [x] Подключение зависимостей (EnhancedInput, GameplayTags, UMG, NetCore)
- [x] Создание структуры папок Content (WoE/Core, Input, Maps, UI и др.)
- [x] Создание структуры папок Source (Core/)

### Часть 3: Настройка Build.cs

- [x] PublicDependencyModuleNames (Core, CoreUObject, Engine, InputCore, EnhancedInput, GameplayTags, UMG, NetCore)
- [x] PrivateDependencyModuleNames (Slate, SlateCore)

### Часть 4: Базовые C++ классы

- [x] AWoE_GameMode — кастомный игровой режим
- [x] AWoE_GameState — состояние игры с GameTimeOfDay (Replicated)
- [x] AWoE_PlayerState — состояние игрока (ClanId, DisplayName — Replicated)
- [x] Настройка сетевой репликации (GetLifetimeReplicatedProps)

### Часть 5: Связь GameMode с классами

- [x] GameStateClass = AWoE_GameState
- [x] PlayerStateClass = AWoE_PlayerState
- [x] DefaultPawnClass = BP_WoE_Character (через ConstructorHelpers)

### Часть 6: Персонаж и камера

- [x] AWoE_Character — базовый персонаж с камерой
- [x] SpringArm + Camera компоненты
- [x] Режим Exploration (top-down, стиль King's Bounty) — v0.2.0
- [x] Режим FirstPerson (вид от первого лица) — v0.2.0
- [x] Переключение режимов камеры (клавиша V) — v0.2.0
- [x] Зум колесом мыши — v0.2.0
- [x] Вращение камеры мышью (yaw) в Exploration — v0.2.0
- [x] Движение WASD относительно камеры — v0.2.0
- [x] Защита от спайков ввода при захвате мыши — v0.2.0
- [x] Абсолютная ротация boom (независимость от вращения персонажа) — v0.2.0
- [x] Принудительная установка настроек компонентов в BeginPlay (защита от Blueprint-перезаписи) — v0.2.0
- [x] FP-FIX: SetRelativeLocation для высоты камеры, bHideMeshInFirstPerson, FirstPersonLookSensitivity — v0.2.1
- [x] FP-FIX: плавный переход pitch при переключении, FInputModeGameOnly, bDoCollisionTest — v0.2.1
- [x] Комментарии в коде переведены на английский — v0.2.1

### Часть 7: Input Assets (UE Editor)

- [x] IA_Move (Axis2D) — WASD
- [x] IA_Look (Axis2D) — Mouse XY
- [x] IA_Zoom (Axis1D) — Mouse Wheel
- [x] IA_ToggleCameraMode (Digital) — клавиша V
- [x] IMC_Default — маппинг контекст со Swizzle YXZ для W/S

### Часть 8: Blueprint-наследник

- [x] BP_WoE_Character (наследник AWoE_Character)
- [x] Привязка IMC_Default и Input Actions
- [x] Mesh (манекен)

### Часть 9: GameMode Blueprint

- [x] BP_WoE_GameMode (наследник AWoE_GameMode)
- [x] Default Pawn Class = BP_WoE_Character
- [x] Назначен в World Settings

### Часть 10: Документация

- [x] ROADMAP.md, CHANGELOG.md
- [x] ARCHITECTURE.md, AI_CONTEXT.md
- [x] DEVELOPER_GUIDE.md, COMMIT_WORKFLOW.md
- [x] DOCS_MAINTENANCE.md, PROMPT_FINALIZE.md

### Чеклист Блока 0

- [x] Проект компилируется без ошибок
- [x] Структура папок создана (Content и Source)
- [x] WoE_GameMode создан и назначен в World Settings
- [x] WoE_GameState создан с GameTimeOfDay (replicated)
- [x] WoE_PlayerState создан с ClanId и DisplayName (replicated)
- [x] WoE_Character создан с камерой (top-down + first person)
- [x] Input Actions и Mapping Context созданы в редакторе
- [x] BP_WoE_Character создан и настроен
- [x] Персонаж двигается WASD
- [x] Камера вращается мышью (yaw)
- [x] Зум работает (колесо мыши)
- [x] V переключает Exploration ↔ First Person
- [ ] Мультиплеер PIE (2 игрока) — протестировать
- [ ] Реализация обновления времени суток (day/night cycle)

---

## Блок 1: Мир — World Partition + Ландшафт (планируется)

- [ ] World Partition для стриминга
- [ ] Ландшафт (Landscape)
- [ ] Базовые биомы (лес, равнина, скалы)

---

## Блок 2: Добыча ресурсов (планируется)

- [ ] Деревья (рубка)
- [ ] Камни (добыча)
- [ ] Система ресурсов (GameplayTags)

---

## Блок 3: Инвентарь + UI (планируется)

- [ ] Система инвентаря (C++)
- [ ] UI инвентаря (UMG)
- [ ] Подбор/выброс предметов

---

## Блок 4: Крафт (планируется)

- [ ] Костёр
- [ ] Инструменты (топор, кирка)
- [ ] Рецепты крафта

---

## Блок 5: Строительство (планируется)

- [ ] Foundation (фундамент)
- [ ] Стены
- [ ] Система привязки (snap)

---

## Блок 6: Характеристики (планируется)

- [ ] HP / Stamina / Hunger
- [ ] Смерть и респавн
- [ ] HUD виджеты

---

## Блок 7: Животные + AI (планируется)

- [ ] AI контроллер для NPC
- [ ] Животные с лутом
- [ ] Агрессивные/пассивные NPC

---

## Блок 8: Сеть (планируется)

- [ ] Сетевые сессии (LAN → EOS)
- [ ] Чат
- [ ] Синхронизация мира

---

## Блок 9: Кланы + торговля (планируется)

- [ ] Система кланов
- [ ] Торговля между игроками
- [ ] Дипломатия

---

## Блок 10: Копание (планируется)

- [ ] Фейк-ямы (визуальные)
- [ ] Размещение ловушек

---

## Зависимости блоков

| Блок | Зависит от |
|------|-----------|
| Блок 1 | Блок 0 |
| Блок 2 | Блок 0 |
| Блок 3 | Блок 2 |
| Блок 4 | Блок 3 |
| Блок 5 | Блок 3 |
| Блок 6 | Блок 3 |
| Блок 7 | Блок 3, Блок 6 |
| Блок 8 | Блоки 0–5 |
| Блок 9 | Блок 8, Блок 3 |
| Блок 10 | Блок 2, Блок 5 |

---

## Сводка по версиям

| Версия | Дата       | Содержание                                     | Статус      |
|--------|------------|------------------------------------------------|-------------|
| v0.2.1 | 19.02.2026 | FP-FIX, bHideMeshInFirstPerson, English comments | Выпущено    |
| v0.2.0 | 19.02.2026 | Камера King's Bounty, top-down + FP, UE 5.7 fix | Выпущено    |
| v0.1.2 | 19.02.2025 | Input Actions, Blueprint (UE)                  | Выпущено    |
| v0.1.1 | 19.02.2025 | docs в git, комментарии                        | Выпущено    |
| v0.1.0 | 19.02.2025 | AWoE_Character с камерой                       | Выпущено    |
| v0.0.2 | 19.02.2025 | Документация и автоматизация                   | Выпущено    |
| v0.0.1 | 19.02.2025 | Документация                                   | Выпущено    |
| v0.0.0 | 19.02.2025 | Инициализация проекта                          | Выпущено    |

---

## Легенда

| Символ | Значение     |
|--------|--------------|
| `[x]`  | Выполнено    |
| `[ ]`  | Запланировано |

---

*Последнее обновление: 19.02.2026 (v0.2.1)*
