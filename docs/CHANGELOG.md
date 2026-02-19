# Changelog

Все значимые изменения в проекте **WorldOfEmpires** документируются в этом файле.

---

## [0.2.0] — 19.02.2026

**Камера King's Bounty** | Полная переработка системы камеры — top-down вид + переключение на First Person

### Изменено

- **Source/WorldOfEmpires/Core/WoE_Character.h** — переработан:
  - `EWoE_CameraMode::Exploration` переименован в "Exploration (Top-Down)"
  - Добавлено свойство `DesiredYaw` (float) — yaw камеры, управляемый мышью
  - Добавлено свойство `ExplorationYawSensitivity` (float) — чувствительность мыши в Exploration
  - `ExplorationPitch` получил meta-ограничение ClampMin/ClampMax (-89..0)
- **Source/WorldOfEmpires/Core/WoE_Character.cpp** — переработан:
  - **Камера King's Bounty:** top-down вид с pitch -55°, arm length 1200, zoom 400–800
  - **Архитектура камеры:** CameraBoom использует `bAbsoluteRotation` (через `SetAbsolute`) и `bUsePawnControlRotation = false` в Exploration — ротация boom полностью независима от controller rotation и вращения персонажа
  - **Tick:** `UpdateCamera` вызывается ДО `Super::Tick` — Spring Arm видит актуальную ротацию в том же кадре
  - **UpdateCamera (Exploration):** управляет boom напрямую через `SetWorldRotation`, не использует controller rotation
  - **OnLook (Exploration):** модифицирует только `DesiredYaw`, не трогает controller rotation
  - **OnMove:** использует `DesiredYaw` для направления движения в Exploration (camera-relative WASD)
  - **BeginPlay:** принудительно устанавливает настройки компонентов CameraBoom (защита от Blueprint-перезаписи старых значений)
  - **ApplyCameraMode:** переключает `bUsePawnControlRotation`, `SetAbsolute`, `bDoCollisionTest`, `bEnableCameraLag` при смене режима
  - **Плавные переходы:** при переключении Exploration↔FirstPerson yaw передаётся между DesiredYaw и controller rotation без рывков
  - **Защита от спайков:** дельты > 200 px отбрасываются (защита при захвате мыши / фокусе окна)
  - Добавлен диагностический `UE_LOG` в BeginPlay

### Исправлено

- Камера больше не переворачивается вверх дном при первом клике мыши
- Устранено дрожание камеры (jitter) из-за конфликта между OnLook и UpdateCamera за controller rotation
- Устранена зависимость boom от вращения персонажа при ходьбе

### Совместимость

- Исправлена ошибка компиляции UE 5.7: `bAbsoluteRotation` стал private — заменён на `SetAbsolute()`
- Исправлена ошибка компиляции UE 5.7: `UE_LOG` с битовыми полями — `FormatStringSan` не позволяет `%d` для bitfield

---

## [0.1.2] — 19.02.2025

**Input Actions, Mapping Context, Blueprint [Только Unreal Engine]** | Настройка ввода и персонажа в редакторе

### Unreal Engine

- **Content/WoE/Input/IMC_Default.uasset** (UE) — Input Mapping Context: привязки WASD (Move), Mouse XY (Look), Mouse Wheel (Zoom), V (Toggle Camera). Swizzle YXZ для W/S
- **Content/WoE/Input/IA_Move.uasset** (UE) — Input Action для движения (2D вектор)
- **Content/WoE/Input/IA_Look.uasset** (UE) — Input Action для обзора (мышь)
- **Content/WoE/Input/IA_Zoom.uasset** (UE) — Input Action для зума (колёсико)
- **Content/WoE/Input/IA_ToggleCameraMode.uasset** (UE) — Input Action для переключения камеры (V)
- **Content/WoE/Core/Characters/BP_WoE_Character.uasset** (UE) — Blueprint-наследник WoE_Character: привязка IMC_Default, Move/Look/Zoom/ToggleCameraMode Actions, Mesh (манекен)
- **Config/DefaultInput.ini** (UE) — настройки Enhanced Input, legacy mappings
- **Config/DefaultEditor.ini** (UE) — настройки редактора
- **Config/DefaultEngine.ini** (UE) — настройки движка

---

## [0.1.1] — 19.02.2025

**Конфигурация и комментарии** | Добавление docs в git, восстановление русских комментариев

### Добавлено

- **.gitignore** — `!/docs/**` для отслеживания папки docs в репозитории
- **docs/** — папка документации (ARCHITECTURE, AI_CONTEXT, CHANGELOG, ROADMAP и др.)

### Изменено

- **Source/WorldOfEmpires/Core/** — восстановлены русские комментарии в коде (WoE_Character, WoE_GameMode, WoE_GameState, WoE_PlayerState)
- **Source/WorldOfEmpires/WorldOfEmpires.Build.cs** — восстановлены русские комментарии

---

## [0.1.0] — 19.02.2025

**Персонаж игрока** | AWoE_Character с камерой и Enhanced Input

### Добавлено

- **Source/WorldOfEmpires/Core/WoE_Character.h** — персонаж:
  - SpringArm + Camera, режимы Exploration / FirstPerson (EWoE_CameraMode)
  - Настройки камеры: CurrentArmLength, MinArmLength, MaxArmLength, ExplorationPitch, ZoomSpeed, CameraInterpSpeed
  - Input Actions: MoveAction, LookAction, ZoomAction, ToggleCameraModeAction
- **Source/WorldOfEmpires/Core/WoE_Character.cpp** — реализация:
  - Движение WASD, вращение камеры мышью, зум колёсиком
  - Переключение режима камеры (V), плавная интерполяция

### Изменено

- **docs/ARCHITECTURE.md** — добавлен WoE_Character в структуру каталогов
- **docs/AI_CONTEXT.md** — добавлен раздел AWoE_Character

---

## [0.0.2] — 19.02.2025

**Документация и автоматизация** | Полная система документации и промпт для ИИ

### Добавлено

- **ARCHITECTURE.md** — правила архитектуры, структура папок, именование (префикс WoE_)
- **AI_CONTEXT.md** — контекст для ИИ: реализованные классы, API, сценарии генерации
- **DEVELOPER_GUIDE.md** — руководство разработчика, workflow, фиксация изменений
- **COMMIT_WORKFLOW.md** — краткий чек-лист для коммитов
- **DOCS_MAINTENANCE.md** — когда обновлять какую документацию
- **PROMPT_FINALIZE.md** — промпт для автоматической финализации (версия, changelog, roadmap, commit)
- **VERSION** — файл с текущей версией (X.Y.Z)

### Изменено

- **ROADMAP.md** — добавлена фаза «Документация и автоматизация»
- **CHANGELOG.md** — ссылка на систему документации

---

## [0.0.1] — 19.02.2025

**Документация** | Roadmap и Changelog

### Добавлено

- **ROADMAP.md** — дорожная карта с фазами, чекбоксами и сводкой по версиям
- **CHANGELOG.md** — журнал изменений с форматом «Добавлено / Изменено / Удалено»

---

## [0.0.0] — 19.02.2025

**Фаза 0–1** | Инициализация проекта, базовая игровая логика

### Добавлено

#### Фаза 0: Инициализация проекта

- **WorldOfEmpires.uproject** — проект Unreal Engine 5.7
- **Source/WorldOfEmpires/WorldOfEmpires.h** — заголовок модуля
- **Source/WorldOfEmpires/WorldOfEmpires.cpp** — регистрация модуля (IMPLEMENT_PRIMARY_GAME_MODULE)
- **Source/WorldOfEmpires/WorldOfEmpires.Build.cs** — конфигурация сборки:
  - PCHUsage: UseExplicitOrSharedPCHs
  - Зависимости: Core, CoreUObject, Engine, InputCore, EnhancedInput, GameplayTags, UMG, NetCore, Slate, SlateCore
- **Source/WorldOfEmpires.Target.cs** — таргет сборки игры (Type: Game)
- **Source/WorldOfEmpiresEditor.Target.cs** — таргет сборки редактора

#### Фаза 1: Базовая игровая логика

- **Source/WorldOfEmpires/Core/WoE_GameMode.h** — заголовок игрового режима
- **Source/WorldOfEmpires/Core/WoE_GameMode.cpp** — реализация:
  - GameStateClass = AWoE_GameState
  - PlayerStateClass = AWoE_PlayerState
  - BeginPlay: лог «WoEGameMode: BeginPlay вызван»
- **Source/WorldOfEmpires/Core/WoE_GameState.h** — заголовок состояния игры
- **Source/WorldOfEmpires/Core/WoE_GameState.cpp** — реализация:
  - GameTimeOfDay (Replicated, BlueprintReadOnly) — время суток, начальное значение 8.0
  - GetLifetimeReplicatedProps: DOREPLIFETIME GameTimeOfDay
- **Source/WorldOfEmpires/Core/WoE_PlayerState.h** — заголовок состояния игрока
- **Source/WorldOfEmpires/Core/WoE_PlayerState.cpp** — реализация:
  - ClanId (Replicated, BlueprintReadOnly) — ID клана, по умолчанию 0
  - DisplayName (Replicated, BlueprintReadOnly) — отображаемое имя, по умолчанию «Survivor»
  - GetLifetimeReplicatedProps: DOREPLIFETIME ClanId, DisplayName

#### Плагины

- **ModelingToolsEditorMode** — включён для Editor

---

## Формат версий

Проект использует [Semantic Versioning](https://semver.org/).

| Версия | Дата       | Содержание                                      |
|--------|------------|-------------------------------------------------|
| 0.2.0  | 19.02.2026 | Камера King's Bounty, top-down + FP, UE 5.7 fix |
| 0.1.2  | 19.02.2025 | Input Actions, Blueprint (UE)                   |
| 0.1.1  | 19.02.2025 | docs в git, комментарии                         |
| 0.1.0  | 19.02.2025 | AWoE_Character с камерой                        |
| 0.0.2  | 19.02.2025 | Документация и автоматизация                    |
| 0.0.1  | 19.02.2025 | Документация                                    |
| 0.0.0  | 19.02.2025 | Инициализация проекта                           |
