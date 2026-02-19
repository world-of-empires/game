# Changelog

Все значимые изменения в проекте **WorldOfEmpires** документируются в этом файле.

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

| Версия | Дата       | Фазы                    |
|--------|------------|--------------------------|
| 0.0.2  | 19.02.2025 | Документация и автоматизация |
| 0.0.1  | 19.02.2025 | Документация             |
| 0.0.0  | 19.02.2025 | 0, 1 (частично)          |
