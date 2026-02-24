# Changelog

Все значимые изменения в проекте **WorldOfEmpires** документируются в этом файле.

---

## [0.2.3] — 19.02.2025

**Click-to-Move (Exploration)** | LMB — бег к точке/за курсором, RMB — вращение камеры

### Добавлено

- **Source/WorldOfEmpires/Core/WoE_Character.h**:
  - `ClickToMoveAction`, `RotateCameraAction` (UInputAction)
  - `bIsClickMoving`, `bIsLMBHeld`, `ClickMoveDestination`, `ClickMoveAcceptanceRadius` (50), `ClickHoldThreshold` (0.2)
  - `bIsRotatingCamera` — флаг вращения камеры RMB
  - `OnClickToMoveStarted`, `OnClickToMoveTriggered`, `OnClickToMoveReleased`
  - `OnRotateCameraStarted`, `OnRotateCameraCompleted`
  - `UpdateClickToMove`, `TraceClickDestination`, `CancelClickToMove`, `ApplyCursorSettings`
- **Source/WorldOfEmpires/Core/WoE_Character.cpp**:
  - **Click-to-move:** LMB клик → trace ground → бег к точке; LMB зажатие → бег за курсором каждый кадр
  - **Приоритет:** LMB зажат → WASD блокируется; LMB не зажат → WASD отменяет click-to-move
  - **RMB:** зажатие — вращение камеры (курсор скрыт, позиция восстанавливается при отпускании)
  - **OnLook:** вращение камеры только при `bIsRotatingCamera` (RMB)
  - **FirstPerson:** click-to-move отключён; курсор скрыт

### Изменено

- **Source/WorldOfEmpires/Core/WoE_Character.cpp** — `OnMove`: при `bIsLMBHeld` WASD игнорируется; `OnLook`: только при RMB

### Unreal Engine

- **Content/Input/Actions/IA_ClickToMove.uasset** (UE) — Input Action (Bool), LMB
- **Content/Input/Actions/IA_RotateCamera.uasset** (UE) — Input Action (Bool), RMB
- **Content/Input/IMC_Default.uasset** (UE) — привязки IA_ClickToMove, IA_RotateCamera
- **Content/Core/Characters/BP_WoE_Character.uasset** (UE) — ClickToMoveAction, RotateCameraAction

---

## [0.2.2] — 24.02.2026

**Two-mesh First Person + Run/Walk + Jump** | Архитектура FP с отдельной камерой и мешем, движение Run/Walk, прыжок

### Добавлено

- **Source/WorldOfEmpires/Core/WoE_Character.h**:
  - `FirstPersonCamera` (UCameraComponent) — отдельная камера на Capsule для FP, `bEnableFirstPersonFieldOfView`, `FirstPersonScale`
  - `FirstPersonMesh` (USkeletalMeshComponent) — меш для FP (руки владельца), `SetOnlyOwnerSee`, `FirstPersonPrimitiveType`, LeaderPose от GetMesh()
  - `ExplorationBoomHeight` (float) — высота pivot Spring Arm (85)
  - `FirstPersonEyeHeight` (float) — высота камеры FP над капсулой (70)
  - `FirstPersonFOV` (float) — FOV для рендера FP-меша (70)
  - `FirstPersonScale` (float) — масштаб FP-меша для предотвращения клиппинга (0.6)
  - `RunSpeed`, `WalkSpeed` (float) — 600 / 300
  - `bIsWalking` (bool) — режим ходьбы
  - `JumpAction`, `WalkAction` (UInputAction)
  - `OnJumpStarted`, `OnJumpCompleted`, `OnWalkStarted`, `OnWalkCompleted`
- **Source/WorldOfEmpires/Core/WoE_Character.cpp**:
  - **Two-mesh FP:** GetMesh() → WorldSpaceRepresentation (тень), FirstPersonMesh → видим владельцу, голова/шея скрыты
  - **FirstPersonCamera** на CapsuleComponent, `bEnableFirstPersonFieldOfView`, `FirstPersonScale`
  - **Run/Walk:** Shift для ходьбы, переключение MaxWalkSpeed
  - **Jump:** привязка JumpAction
  - `ExplorationBoomHeight` вместо FirstPersonCameraHeight для boom
  - `VisibilityBasedAnimTickOption = AlwaysTickPoseAndRefreshBones` для LeaderPose

### Изменено

- **Source/WorldOfEmpires/Core/WoE_Character.h** — `FirstPersonCameraHeight` переименован в `FirstPersonEyeHeight`; удалён `bHideMeshInFirstPerson`
- **Source/WorldOfEmpires/Core/WoE_Character.cpp** — ApplyCameraMode: переключение FollowCamera/FirstPersonCamera, FirstPersonMesh visibility

### Unreal Engine

- **Config/DefaultEngine.ini** (UE) — GameDefaultMap, EditorStartupMap = MainMap; ActiveGameNameRedirects TP_FirstPerson
- **Config/DefaultInput.ini** (UE) — PrimaryAction (LMB, RightTrigger), ConsoleKeys=ё
- **Content/Core/Characters/BP_WoE_Character.uasset** (UE) — JumpAction, WalkAction, FirstPersonMesh
- **Content/Input/IMC_Default.uasset** (UE) — привязки Jump, Walk
- **Content/Input/Actions/IA_Walk.uasset** (UE) — Input Action для ходьбы
- **Content/Maps/** (UE) — MainMap
- **Content/__ExternalActors__/**, **Content/__ExternalObjects__/** (UE) — данные уровней

### Удалено

- **Content/Core/Characters/Mannequins/Anims/Death/MM_Death_Back_01.uasset**, **MM_Death_Front_01.uasset** (UE)
- **Content/Input/Actions/IA_Sprint.uasset** (UE) — заменён на IA_Walk

### Документация

- **docs/AI_CONTEXT.md** — FirstPersonCamera, FirstPersonMesh, RunSpeed, WalkSpeed, JumpAction, WalkAction
- **docs/ARCHITECTURE.md** — версия 0.2.2
- **docs/ROADMAP.md** — версия, пункты two-mesh FP, Run/Walk, Jump
- **docs/DEVELOPER_GUIDE.md** — версия 0.2.2
- **docs/PROMPT_FINALIZE_NO_COMMIT.md** — правки
- **docs/VERSION** — 0.2.2

---

## [0.2.1] — 22.02.2026

**First Person Fix + English Comments** | Исправление FP-режима, перевод комментариев на английский

### Изменено

- **Source/WorldOfEmpires/Core/WoE_Character.h**:
  - Добавлено `bHideMeshInFirstPerson` (bool) — скрывать меш от владельца в FP (нет клиппинга тела)
  - Добавлено `FirstPersonLookSensitivity` (float) — отдельная чувствительность мыши для FP
  - Категория `WoE|Camera|FirstPerson` для FP-настроек
  - Комментарии переведены на английский
- **Source/WorldOfEmpires/Core/WoE_Character.cpp**:
  - **FP-FIX:** Высота камеры через `SetRelativeLocation` на boom (единая для обоих режимов)
  - **FP-FIX:** `bDoCollisionTest=true` — камера не проходит сквозь стены в Exploration
  - **FP-FIX:** При переключении в FP — передача `DesiredPitch` в `SetControlRotation` (нет рывка камеры)
  - **FP-FIX:** `FirstPersonLookSensitivity` для мыши в FP
  - **FP-FIX:** `bHideMeshInFirstPerson` → `SetOwnerNoSee(true)` — тело скрыто в FP (руки видны при `false`)
  - **FP-FIX:** `bShowMouseCursor=false`, `FInputModeGameOnly` в BeginPlay
  - **FP-FIX:** При переключении Exploration←FP: `DesiredYaw` из `Controller->GetControlRotation().Yaw`
  - Комментарии переведены на английский

### Документация

- **docs/AI_CONTEXT.md** — обновлён раздел AWoE_Character (bHideMeshInFirstPerson, FirstPersonLookSensitivity, архитектура камеры)
- **docs/CHANGELOG.md** — добавлена запись 0.2.1
- **docs/ROADMAP.md** — обновлена версия, добавлены пункты FP-FIX
- **docs/ARCHITECTURE.md** — обновлена версия
- **docs/VERSION** — 0.2.1

---

## [0.2.0] — 19.02.2026

**Камера King's Bounty** | Полная переработка системы камеры — top-down вид + переключение на First Person

### Изменено

- **Source/WorldOfEmpires/Core/WoE_Character.h** — переработан:
  - `EWoE_CameraMode::Exploration` переименован в "Exploration (Top-Down)"
  - Добавлено свойство `DesiredYaw` (float) — yaw камеры, управляемый мышью
  - Добавлено свойство `DesiredPitch` (float) — pitch камеры в Exploration (управляется мышью)
  - Добавлено свойство `FirstPersonCameraHeight` (float) — высота камеры в First Person (70, Clamp 0–150)
  - Добавлено свойство `ExplorationYawSensitivity` (float) — чувствительность мыши (1.0, Clamp 0.1–5.0)
  - `ExplorationPitch` получил meta-ограничение ClampMin/ClampMax (-89..0)
- **Source/WorldOfEmpires/Core/WoE_Character.cpp** — переработан:
  - **Камера King's Bounty:** top-down вид с pitch -55°, arm length 1200, zoom 400–800
  - **Архитектура камеры:** CameraBoom использует `bAbsoluteRotation` (через `SetAbsolute`) и `bUsePawnControlRotation = false` в Exploration — ротация boom полностью независима от controller rotation и вращения персонажа
  - **Tick:** `UpdateCamera` вызывается ДО `Super::Tick` — Spring Arm видит актуальную ротацию в том же кадре
  - **UpdateCamera (Exploration):** управляет boom через `SetWorldRotation` (pitch=DesiredPitch, yaw=DesiredYaw)
  - **OnLook (Exploration):** модифицирует `DesiredYaw` и `DesiredPitch` (Clamp -89..-5), не трогает controller rotation
  - **Zoom:** MinArmLength 300, MaxArmLength 2500, TargetArmLength 1200
  - **OnMove:** использует `DesiredYaw` для направления движения в Exploration (camera-relative WASD)
  - **BeginPlay:** принудительно устанавливает настройки компонентов CameraBoom (защита от Blueprint-перезаписи старых значений)
  - **ApplyCameraMode:** переключает `bUsePawnControlRotation`, `SetAbsolute`, `bDoCollisionTest`, `bEnableCameraLag`; First Person — TargetOffset/SocketOffset для высоты камеры
  - **Плавные переходы:** при переключении Exploration↔FirstPerson yaw и pitch передаются между DesiredYaw/DesiredPitch и controller rotation без рывков
  - **Защита от спайков:** дельты > 200 px отбрасываются (защита при захвате мыши / фокусе окна)
  - Добавлен диагностический `UE_LOG` в BeginPlay

### Исправлено

- Камера больше не переворачивается вверх дном при первом клике мыши
- Устранено дрожание камеры (jitter) из-за конфликта между OnLook и UpdateCamera за controller rotation
- Устранена зависимость boom от вращения персонажа при ходьбе

### Unreal Engine

- **Config/DefaultEngine.ini** (UE) — GlobalDefaultGameMode = BP_WoE_GameMode
- **Content/Core/Characters/BP_WoE_Character.uasset** (UE) — обновление настроек
- **Content/Input/IMC_Default.uasset** (UE) — обновление маппинга
- **Content/Core/Characters/Mannequins/** (UE) — обновление мешей (SKM_Manny, SKM_Quinn), анимаций, ригов
- **Content/__ExternalActors__/** (UE) — удалены шаблонные уровни Variant_Combat, Variant_SideScrolling

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

| Версия  | Дата       | Содержание                                      |
|---------|------------|-------------------------------------------------|
| v0.2.2  | 24.02.2026 | Two-mesh FP, Run/Walk, Jump, IA_Walk, MainMap   |
| v0.2.1  | 22.02.2026 | FP-FIX, bHideMeshInFirstPerson, English comments|
| v0.2.0  | 19.02.2026 | Камера King's Bounty, top-down + FP, UE 5.7 fix |
| v0.1.2  | 19.02.2025 | Input Actions, Blueprint (UE)                   |
| v0.1.1  | 19.02.2025 | docs в git, комментарии                         |
| v0.1.0  | 19.02.2025 | AWoE_Character с камерой                        |
| v0.0.2  | 19.02.2025 | Документация и автоматизация                    |
| v0.0.1  | 19.02.2025 | Документация                                    |
| v0.0.0  | 19.02.2025 | Инициализация проекта                           |
