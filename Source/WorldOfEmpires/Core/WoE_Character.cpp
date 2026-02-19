
#include "WoE_Character.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"

AWoE_Character::AWoE_Character()
{
    PrimaryActorTick.bCanEverTick = true;

    // ---- Spring Arm (camera boom) ----
    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(RootComponent);
    CameraBoom->bUsePawnControlRotation = false;
    CameraBoom->SetAbsolute(false, true, false);
    //CameraBoom->TargetArmLength = 1200.0f;
    CameraBoom->bDoCollisionTest = false;
    CameraBoom->bEnableCameraLag = true;
    CameraBoom->CameraLagSpeed = 8.0f;

    // ---- Camera ----
    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
    FollowCamera->bUsePawnControlRotation = false;

    // ---- Character rotation ----
    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);

    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = false;
    bUseControllerRotationRoll = false;

    // ---- Default camera values (King's Bounty top-down style) ----
    CurrentCameraMode = EWoE_CameraMode::Exploration;
    CurrentArmLength = 400.0f;
    MinArmLength = 400.0f;
    MaxArmLength = 800.0f;
    ExplorationPitch = -55.0f;
    DesiredYaw = 0.0f;
    ExplorationYawSensitivity = 0.3f;
    ZoomSpeed = 80.0f;
    CameraInterpSpeed = 8.0f;
}

// ================================================================
// BEGIN PLAY
// ================================================================
void AWoE_Character::BeginPlay()
{
    Super::BeginPlay();

    // Force component settings at runtime.
    // Blueprint can save old property values that override the C++ constructor.
    // Setting them here guarantees correct values regardless of Blueprint state.
    if (CameraBoom)
    {
        CameraBoom->bUsePawnControlRotation = false;
        CameraBoom->SetAbsolute(false, true, false);
        CameraBoom->bDoCollisionTest = false;
        CameraBoom->bEnableCameraLag = true;
        CameraBoom->CameraLagSpeed = 8.0f;
        CameraBoom->TargetArmLength = CurrentArmLength;
    }

    DesiredYaw = GetActorRotation().Yaw;

    // Set camera rotation immediately so the first frame is correct.
    if (CameraBoom)
    {
        CameraBoom->SetWorldRotation(FRotator(ExplorationPitch, DesiredYaw, 0.0f));
    }

    if (APlayerController* PC = Cast<APlayerController>(Controller))
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
            ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
        {
            if (DefaultMappingContext)
            {
                Subsystem->AddMappingContext(DefaultMappingContext, 0);
            }
        }
    }

    ApplyCameraMode(CurrentCameraMode);

    UE_LOG(LogTemp, Warning, TEXT("WoE Camera: Pitch=%.1f Yaw=%.1f Arm=%.1f PawnCtrlRot=%s"),
        ExplorationPitch, DesiredYaw, CurrentArmLength,
        CameraBoom && CameraBoom->bUsePawnControlRotation ? TEXT("true") : TEXT("false"));
}

// ================================================================
// TICK
// ================================================================
void AWoE_Character::Tick(float DeltaTime)
{
    // Update camera BEFORE Super::Tick so the spring arm component
    // sees our rotation when it ticks (inside Super::Tick).
    UpdateCamera(DeltaTime);

    Super::Tick(DeltaTime);
}

// ================================================================
// INPUT BINDING
// ================================================================
void AWoE_Character::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    if (UEnhancedInputComponent* EnhancedInput =
        CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
    {
        if (MoveAction)
        {
            EnhancedInput->BindAction(
                MoveAction, ETriggerEvent::Triggered, this, &AWoE_Character::OnMove);
        }
        if (LookAction)
        {
            EnhancedInput->BindAction(
                LookAction, ETriggerEvent::Triggered, this, &AWoE_Character::OnLook);
        }
        if (ZoomAction)
        {
            EnhancedInput->BindAction(
                ZoomAction, ETriggerEvent::Triggered, this, &AWoE_Character::OnZoom);
        }
        if (ToggleCameraModeAction)
        {
            EnhancedInput->BindAction(
                ToggleCameraModeAction, ETriggerEvent::Started,
                this, &AWoE_Character::OnToggleCameraMode);
        }
    }
}

// ================================================================
// INPUT HANDLERS
// ================================================================

void AWoE_Character::OnMove(const FInputActionValue& Value)
{
    const FVector2D MovementVector = Value.Get<FVector2D>();
    if (Controller == nullptr) return;

    const float DirectionYaw =
        (CurrentCameraMode == EWoE_CameraMode::Exploration)
        ? DesiredYaw
        : Controller->GetControlRotation().Yaw;

    const FRotator YawRotation(0.0f, DirectionYaw, 0.0f);
    const FVector ForwardDir = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
    const FVector RightDir   = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

    AddMovementInput(ForwardDir, MovementVector.Y);
    AddMovementInput(RightDir, MovementVector.X);
}

void AWoE_Character::OnLook(const FInputActionValue& Value)
{
    FVector2D LookAxisVector = Value.Get<FVector2D>();
    if (Controller == nullptr) return;

    // Discard abnormally large deltas (mouse capture / window focus spike).
    if (FMath::Abs(LookAxisVector.X) > 200.0f || FMath::Abs(LookAxisVector.Y) > 200.0f)
    {
        return;
    }

    if (CurrentCameraMode == EWoE_CameraMode::FirstPerson)
    {
        AddControllerYawInput(LookAxisVector.X);
        AddControllerPitchInput(LookAxisVector.Y);
    }
    else
    {
        DesiredYaw += LookAxisVector.X * ExplorationYawSensitivity;
    }
}

void AWoE_Character::OnZoom(const FInputActionValue& Value)
{
    if (CurrentCameraMode != EWoE_CameraMode::Exploration) return;

    const float ZoomValue = Value.Get<float>();
    CurrentArmLength -= ZoomValue * ZoomSpeed;
    CurrentArmLength = FMath::Clamp(CurrentArmLength, MinArmLength, MaxArmLength);
}

void AWoE_Character::OnToggleCameraMode(const FInputActionValue& Value)
{
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
// CAMERA ? per-frame update
// ================================================================
void AWoE_Character::UpdateCamera(float DeltaTime)
{
    if (!CameraBoom) return;

    if (CurrentCameraMode == EWoE_CameraMode::Exploration)
    {
        // Smooth zoom.
        CameraBoom->TargetArmLength = FMath::FInterpTo(
            CameraBoom->TargetArmLength, CurrentArmLength, DeltaTime, CameraInterpSpeed);

        // Set boom world rotation directly ? completely independent
        // of controller rotation. This avoids jitter from legacy input
        // or PlayerController fighting over the rotation.
        const FRotator CurrentRot = CameraBoom->GetComponentRotation();
        const float NewPitch = FMath::FInterpTo(
            CurrentRot.Pitch, ExplorationPitch, DeltaTime, CameraInterpSpeed);
        CameraBoom->SetWorldRotation(FRotator(NewPitch, DesiredYaw, 0.0f));
    }
    else if (CurrentCameraMode == EWoE_CameraMode::FirstPerson)
    {
        CameraBoom->TargetArmLength = FMath::FInterpTo(
            CameraBoom->TargetArmLength, 0.0f, DeltaTime, CameraInterpSpeed);
        // Rotation handled by bUsePawnControlRotation + AddControllerYaw/PitchInput.
    }
}

// ================================================================
// CAMERA ? apply mode
// ================================================================
void AWoE_Character::ApplyCameraMode(EWoE_CameraMode NewMode)
{
    CurrentCameraMode = NewMode;

    switch (NewMode)
    {
    case EWoE_CameraMode::Exploration:
    {
        GetCharacterMovement()->bOrientRotationToMovement = true;
        bUseControllerRotationYaw = false;

        CameraBoom->bUsePawnControlRotation = false;
        CameraBoom->SetAbsolute(false, true, false);
        CameraBoom->bDoCollisionTest = false;
        CameraBoom->bEnableCameraLag = true;

        // Capture current camera yaw so there's no snap on mode switch.
        DesiredYaw = CameraBoom->GetComponentRotation().Yaw;

        if (CurrentArmLength < MinArmLength + 50.0f)
        {
            CurrentArmLength = 1200.0f;
        }

        UE_LOG(LogTemp, Log, TEXT("Camera: Exploration (Top-Down) mode"));
        break;
    }

    case EWoE_CameraMode::FirstPerson:
    {
        GetCharacterMovement()->bOrientRotationToMovement = false;
        bUseControllerRotationYaw = true;

        CameraBoom->bUsePawnControlRotation = true;
        CameraBoom->SetAbsolute(false, false, false);
        CameraBoom->bDoCollisionTest = true;
        CameraBoom->bEnableCameraLag = false;

        // Hand off current camera yaw to controller for FP look.
        if (Controller)
        {
            Controller->SetControlRotation(FRotator(0.0f, DesiredYaw, 0.0f));
        }

        CurrentArmLength = 0.0f;

        UE_LOG(LogTemp, Log, TEXT("Camera: First Person mode"));
        break;
    }
    }
}
