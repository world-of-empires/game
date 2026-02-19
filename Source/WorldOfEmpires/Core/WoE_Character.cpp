
#include "WoE_Character.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"

AWoE_Character::AWoE_Character()
{
    // ---- Настройка Spring Arm (штанги камеры) ----

    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));

    CameraBoom->SetupAttachment(RootComponent);
    CameraBoom->bUsePawnControlRotation = true; // bUsePawnControlRotation — если true, штанга вращается вместе
    CameraBoom->TargetArmLength = 400.0f;

    // ---- Настройка камеры ----

    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));

    FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
    FollowCamera->bUsePawnControlRotation = false;

    // ---- Настройка вращения персонажа ----
    
    GetCharacterMovement()->bOrientRotationToMovement = true; // bOrientRotationToMovement = true — персонаж поворачивается 
    GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // FRotator(Pitch, Yaw, Roll):

    bUseControllerRotationPitch = false; // наклон вперёд/назад
    bUseControllerRotationYaw = false; // поворот влево/вправо (540 = быстрый поворот)
    bUseControllerRotationRoll = false; //  наклон вбок

    // ---- Значения камеры по умолчанию ----
    CurrentCameraMode = EWoE_CameraMode::Exploration;
    CurrentArmLength = 400.0f;
    MinArmLength = 150.0f;    // Близко (как Diablo, но ещё ближе)
    MaxArmLength = 800.0f;    // Далеко
    ExplorationPitch = -35.0f; // Угол сверху (отрицательный = смотрим вниз)
    ZoomSpeed = 50.0f;         // Шаг зума за один "щелчок" колеса
    CameraInterpSpeed = 5.0f;  // Плавность (больше = быстрее)

 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// ================================================================
// BEGIN PLAY
// ================================================================
void AWoE_Character::BeginPlay()
{
    Super::BeginPlay();

    // Регистрируем Input Mapping Context.
    // Mapping Context говорит движку: "Клавиша W привязана к MoveAction" и т.д.

    // Получаем PlayerController этого персонажа.
    // Cast — "приведение типа": проверяем что контроллер — именно PlayerController.
    if (APlayerController* PC = Cast<APlayerController>(Controller))
    {
        // Получаем подсистему Enhanced Input для этого контроллера.
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
            ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
        {
            // Добавляем наш Mapping Context с приоритетом 0 (по умолчанию).
            if (DefaultMappingContext)
            {
                Subsystem->AddMappingContext(DefaultMappingContext, 0);
            }
        }
    }

    // Применяем начальный режим камеры.
    ApplyCameraMode(CurrentCameraMode);
}

// ================================================================
// TICK (каждый кадр)
// ================================================================
void AWoE_Character::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    UpdateCamera(DeltaTime); // Плавно обновляем камеру каждый кадр.
}

// ================================================================
// ПРИВЯЗКА ВВОДА
// ================================================================
void AWoE_Character::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

    // Приводим к Enhanced Input Component.
    // CastChecked — как Cast, но если не удалось — крашит игру.
    // Используем потому что Enhanced Input ДОЛЖЕН быть включён
    if (UEnhancedInputComponent* EnhancedInput = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {
        // BindAction — связывает Input Action с нашей функцией.
        // ETriggerEvent::Triggered — вызывается КАЖДЫЙ КАДР пока клавиша зажата.
        
        if (MoveAction) {
            EnhancedInput->BindAction(
                MoveAction,                          // Какое действие
                ETriggerEvent::Triggered,            // Когда вызывать
                this,                                // Кто обрабатывает
                &AWoE_Character::OnMove              // Какую функцию вызвать
            );
        }

        if (LookAction) {
            EnhancedInput->BindAction(LookAction, ETriggerEvent::Triggered, this, &AWoE_Character::OnLook);
        }

        if (ZoomAction) {
            EnhancedInput->BindAction(ZoomAction, ETriggerEvent::Triggered, this, &AWoE_Character::OnZoom);
        }

        if (ToggleCameraModeAction) {
            // Started — вызывается ОДИН раз при нажатии (не при удержании).
            EnhancedInput->BindAction(ToggleCameraModeAction, ETriggerEvent::Started, this, &AWoE_Character::OnToggleCameraMode);
        }
    }

}


// ================================================================
// ОБРАБОТЧИКИ ВВОДА
// ================================================================
void AWoE_Character::OnMove(const FInputActionValue& Value)
{
    // Получаем 2D вектор из ввода.
    // Для WASD: X = вперёд/назад (W/S), Y = влево/вправо (A/D).
    const FVector2D MovementVector = Value.Get<FVector2D>();

    if (Controller == nullptr) return;
    
    const FRotator Rotation = Controller->GetControlRotation(); // Получаем куда смотрит камера (поворот контроллера).
    const FRotator YawRotation(0, Rotation.Yaw, 0); // Берём только Yaw (поворот влево/вправо), игнорируем наклон.

    // Вычисляем направления "вперёд" и "вправо" относительно камеры.
    // FRotationMatrix — матрица вращения (математика преобразований).
    // GetUnitAxis(EAxis::X) — единичный вектор "вперёд" для данного поворота.
    const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
    const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

    // AddMovementInput — стандартная функция ACharacter.
    // Она передаёт направление в CharacterMovementComponent,
    // который обрабатывает физику, скорость, гравитацию и т.д.
    // Второй параметр — масштаб (1.0 = полная скорость).
    AddMovementInput(ForwardDirection, MovementVector.X);
    AddMovementInput(RightDirection, MovementVector.Y);
}

void AWoE_Character::OnLook(const FInputActionValue& Value)
{
    // Вращение камеры мышью.
    const FVector2D LookAxisVector = Value.Get<FVector2D>();

    if (Controller == nullptr) return;

    if (CurrentCameraMode == EWoE_CameraMode::FirstPerson)
    {
        // В режиме от первого лица: мышь вращает камеру свободно.
        AddControllerYawInput(LookAxisVector.X);    // Влево/вправо
        AddControllerPitchInput(LookAxisVector.Y);  // Вверх/вниз
    }
    else // Exploration
    {
        // В Exploration: мышь вращает камеру вокруг персонажа
        // (только если зажата правая кнопка мыши — это можно настроить
        // в Input Action, но для MVP вращаем всегда).
        AddControllerYawInput(LookAxisVector.X);
        // Pitch НЕ меняем в Exploration — угол фиксирован.
    }
}


void AWoE_Character::OnZoom(const FInputActionValue& Value)
{
    // Колесо мыши — приближение/отдаление.
    const float ZoomValue = Value.Get<float>();

    // Изменяем целевую длину штанги.
    CurrentArmLength -= ZoomValue * ZoomSpeed;
    // Минус потому что "scroll up" = приближение (уменьшение длины).

    // FMath::Clamp — ограничивает значение в диапазоне [Min, Max].
    CurrentArmLength = FMath::Clamp(CurrentArmLength, MinArmLength, MaxArmLength);

    // Если приблизили очень сильно — автоматически переходим в First Person.
    if (CurrentArmLength <= MinArmLength + 10.0f)
    {
        // Можно раскомментировать для автоматического переключения:
        // ApplyCameraMode(EWoECameraMode::FirstPerson);
    }
}

void AWoE_Character::OnToggleCameraMode(const FInputActionValue& Value)
{
    // Переключение режима камеры (кнопка V).
    if (CurrentCameraMode == EWoE_CameraMode::Exploration)
    {
        ApplyCameraMode(EWoE_CameraMode::FirstPerson);
    }
    else
    {
        ApplyCameraMode(EWoE_CameraMode::Exploration);
    }
}

// ================================================================
// КАМЕРА — обновление каждый кадр
// ================================================================
void AWoE_Character::UpdateCamera(float DeltaTime)
{
    if (!CameraBoom) return;

    if (CurrentCameraMode == EWoE_CameraMode::Exploration)
    {
        // Плавно интерполируем длину штанги к целевой.
        // FInterpTo — плавный переход от текущего к целевому значению.
        // InterpSpeed — чем больше, тем быстрее.
        const float NewArmLength = FMath::FInterpTo(
            CameraBoom->TargetArmLength,    // Текущее значение
            CurrentArmLength,                // Целевое значение
            DeltaTime,                       // Время кадра
            CameraInterpSpeed                // Скорость
        );
        CameraBoom->TargetArmLength = NewArmLength;

        // Фиксируем угол наклона (pitch) для Exploration.
        // Получаем текущий поворот контроллера.
        FRotator ControlRot = Controller ? Controller->GetControlRotation() : FRotator::ZeroRotator;

        // Плавно двигаем pitch к ExplorationPitch.
        const float NewPitch = FMath::FInterpTo(
            ControlRot.Pitch,
            ExplorationPitch,
            DeltaTime,
            CameraInterpSpeed
        );
        ControlRot.Pitch = NewPitch;

        if (Controller)
        {
            Controller->SetControlRotation(ControlRot);
        }
    }
    else if (CurrentCameraMode == EWoE_CameraMode::FirstPerson)
    {
        // В First Person штанга = 0 (камера внутри головы).
        const float NewArmLength = FMath::FInterpTo(
            CameraBoom->TargetArmLength,
            0.0f,
            DeltaTime,
            CameraInterpSpeed
        );
        CameraBoom->TargetArmLength = NewArmLength;
    }
}

// ================================================================
// КАМЕРА — применение режима
// ================================================================
void AWoE_Character::ApplyCameraMode(EWoE_CameraMode NewMode)
{
    CurrentCameraMode = NewMode;

    switch (NewMode)
    {
    case EWoE_CameraMode::Exploration:
        // Персонаж поворачивается к направлению движения.
        GetCharacterMovement()->bOrientRotationToMovement = true;
        bUseControllerRotationYaw = false;

        // Восстанавливаем дистанцию (если были в First Person).
        if (CurrentArmLength < MinArmLength + 50.0f)
        {
            CurrentArmLength = 400.0f; // Дистанция по умолчанию
        }

        UE_LOG(LogTemp, Log, TEXT("Камера: Exploration mode"));
        break;

    case EWoE_CameraMode::FirstPerson:
        // В First Person персонаж смотрит куда смотрит камера.
        GetCharacterMovement()->bOrientRotationToMovement = false;
        bUseControllerRotationYaw = true;

        CurrentArmLength = 0.0f;

        UE_LOG(LogTemp, Log, TEXT("Камера: First Person mode"));
        break;
    }
}