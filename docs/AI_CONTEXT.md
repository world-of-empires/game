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

### 2.4. AWoE_Character

**Файлы:** `Source/WorldOfEmpires/Core/WoE_Character.h`, `WoE_Character.cpp`

| Свойство | Тип | Описание |
|----------|-----|----------|
| `CameraBoom` | USpringArmComponent | Spring Arm — `SetAbsolute(rotation=true)`, `bUsePawnControlRotation=false` в Exploration |
| `FollowCamera` | UCameraComponent | Камера, прикреплена к концу boom |
| `CurrentCameraMode` | EWoE_CameraMode | Exploration (Top-Down) / FirstPerson |
| `CurrentArmLength` | float | Текущая дистанция камеры (1200 по умолчанию) |
| `MinArmLength` | float | Мин. зум (300) |
| `MaxArmLength` | float | Макс. зум (2500) |
| `ExplorationPitch` | float | Начальный угол pitch top-down (-55°, ClampMin=-89, ClampMax=0) |
| `DesiredYaw` | float | Yaw камеры в Exploration (управляется мышью) |
| `DesiredPitch` | float | Pitch камеры в Exploration (управляется мышью, Clamp -89..-5) |
| `FirstPersonCameraHeight` | float | Высота камеры над корнем капсулы (70, Clamp 0–200) |
| `bHideMeshInFirstPerson` | bool | Скрывать меш от владельца в FP (true = нет клиппинга тела) |
| `FirstPersonLookSensitivity` | float | Чувствительность мыши в FP (1.0, Clamp 0.1–5.0) |
| `ExplorationYawSensitivity` | float | Чувствительность мыши в Exploration (1.0, Clamp 0.1–5.0) |
| `ZoomSpeed` | float | Скорость зума (80) |
| `CameraInterpSpeed` | float | Скорость интерполяции камеры (8) |
| `DefaultMappingContext` | UInputMappingContext | IMC_Default |
| `MoveAction`, `LookAction`, `ZoomAction`, `ToggleCameraModeAction` | UInputAction | Input Actions |

**Методы:**

| Метод | Описание |
|-------|----------|
| `OnMove` | WASD движение; в Exploration — относительно DesiredYaw, в FP — относительно controller rotation |
| `OnLook` | Мышь; в Exploration — DesiredYaw/DesiredPitch; в FP — AddControllerYaw/PitchInput с FirstPersonLookSensitivity |
| `OnZoom` | Колесо мыши; изменяет CurrentArmLength (только в Exploration) |
| `OnToggleCameraMode` | Переключает Exploration ↔ FirstPerson |
| `UpdateCamera` | Каждый кадр (до Super::Tick); в Exploration — SetWorldRotation на boom; в FP — arm→0 |
| `ApplyCameraMode` | Переключает bUsePawnControlRotation, SetAbsolute, collision, lag; синхронизирует yaw/pitch; SetOwnerNoSee при bHideMeshInFirstPerson |

**Архитектура камеры:**
- **Exploration:** boom вращается через `SetWorldRotation` (pitch=DesiredPitch, yaw=DesiredYaw). Boom `SetAbsolute(rotation=true)` — независим от персонажа. `bDoCollisionTest=true` — камера не проходит сквозь стены. Высота boom — `SetRelativeLocation(Z=FirstPersonCameraHeight)`.
- **FirstPerson:** boom `bUsePawnControlRotation=true`, arm length=0. Мышь управляет controller rotation. `bHideMeshInFirstPerson` → `SetOwnerNoSee(true)` — тело скрыто от владельца. Курсор скрыт, `FInputModeGameOnly`.
- **Переключение:** DesiredYaw/DesiredPitch ↔ Controller Rotation для плавного перехода без рывков.

**Использование:** Blueprint **BP_WoE_Character** наследует AWoE_Character и задаёт IMC_Default + Actions. В World Settings → Default Pawn Class = BP_WoE_Character.

---

### 2.5. Unreal Engine — ассеты (Content)

**Input (Content/WoE/Input/):**

| Ассет | Тип | Описание |
|-------|-----|----------|
| `IMC_Default` | Input Mapping Context | WASD (Move), Mouse XY (Look), Mouse Wheel (Zoom), V (Toggle Camera). Swizzle YXZ для W/S |
| `IA_Move` | Input Action | 2D вектор движения |
| `IA_Look` | Input Action | 2D вектор обзора (мышь) |
| `IA_Zoom` | Input Action | Зум (колёсико) |
| `IA_ToggleCameraMode` | Input Action | Переключение камеры (V) |

**Blueprint (Content/WoE/Core/Characters/):**

| Ассет | Родитель | Описание |
|-------|----------|----------|
| `BP_WoE_Character` | AWoE_Character | Привязка IMC_Default, Move/Look/Zoom/ToggleCameraMode Actions, Mesh (манекен) |

---

## 3. Зависимости (WorldOfEmpires.Build.cs)

| Модуль | Назначение |
|--------|------------|
| Core, CoreUObject, Engine | Базовые типы UE |
| InputCore, EnhancedInput | Ввод (AWoE_Character, IMC_Default, IA_*) |
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

### Добавить персонажа (AWoE_Character)

1. Создать `WoE_Character.h/cpp` в `Core/`
2. Наследовать от `ACharacter`
3. Создать Blueprint BP_WoE_Character, привязать IMC_Default и Input Actions
4. В World Settings → Default Pawn Class = BP_WoE_Character (или в GameMode)
5. Добавить в AI_CONTEXT.md

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

*Документ актуален для версии 0.2.1. Обновляй при добавлении нового API.*

**Как обновлять:** см. DEVELOPER_GUIDE.md (раздел «Как правильно зафиксировать изменения»)
