
#include "WoE_Character.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"

AWoE_Character::AWoE_Character()
{
    // ---- Spring Arm setup (camera boom) ----

    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));

    CameraBoom->SetupAttachment(RootComponent);
    CameraBoom->bUsePawnControlRotation = true; // If true, boom rotates with controller
    CameraBoom->TargetArmLength = 400.0f;

    // ---- Camera setup ----

    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));

    FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
    FollowCamera->bUsePawnControlRotation = false;

    // ---- Character rotation setup ----
    
    GetCharacterMovement()->bOrientRotationToMovement = true; // Character rotates toward movement direction
    GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // FRotator(Pitch, Yaw, Roll)

    bUseControllerRotationPitch = false; // Pitch forward/back
    bUseControllerRotationYaw = false;   // Yaw left/right (540 = fast turn)
    bUseControllerRotationRoll = false; // Roll sideways

    // ---- Default camera values ----
    CurrentCameraMode = EWoE_CameraMode::Exploration;
    CurrentArmLength = 400.0f;
    MinArmLength = 150.0f;    // Close (like Diablo, but closer)
    MaxArmLength = 800.0f;    // Far
    ExplorationPitch = -35.0f; // Angle from above (negative = looking down)
    ZoomSpeed = 50.0f;         // Zoom step per one wheel "click"
    CameraInterpSpeed = 5.0f;  // Smoothness (higher = faster)

 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// ================================================================
// BEGIN PLAY
// ================================================================
void AWoE_Character::BeginPlay()
{
    Super::BeginPlay();

    // Register Input Mapping Context.
    // Mapping Context tells the engine: "Key W is bound to MoveAction" etc.

    // Get this character's PlayerController.
    // Cast - "type conversion": verify that controller is indeed PlayerController.
    if (APlayerController* PC = Cast<APlayerController>(Controller))
    {
        // Get Enhanced Input subsystem for this controller.
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
            ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
        {
            // Add our Mapping Context with priority 0 (default).
            if (DefaultMappingContext)
            {
                Subsystem->AddMappingContext(DefaultMappingContext, 0);
            }
        }
    }

    // Apply initial camera mode.
    ApplyCameraMode(CurrentCameraMode);
}

// ================================================================
// TICK (every frame)
// ================================================================
void AWoE_Character::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    UpdateCamera(DeltaTime); // Smoothly update camera every frame.
}

// ================================================================
// INPUT BINDING
// ================================================================
void AWoE_Character::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

    // Cast to Enhanced Input Component.
    // CastChecked - like Cast, but crashes the game if it fails.
    // We use it because Enhanced Input MUST be enabled.
    if (UEnhancedInputComponent* EnhancedInput = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {
        // BindAction - binds Input Action to our function.
        // ETriggerEvent::Triggered - called EVERY FRAME while key is held.
        
        if (MoveAction) {
            EnhancedInput->BindAction(
                MoveAction,                          // Which action
                ETriggerEvent::Triggered,            // When to call
                this,                                // Who handles it
                &AWoE_Character::OnMove              // Which function to call
            );
        }

        if (LookAction) {
            EnhancedInput->BindAction(LookAction, ETriggerEvent::Triggered, this, &AWoE_Character::OnLook);
        }

        if (ZoomAction) {
            EnhancedInput->BindAction(ZoomAction, ETriggerEvent::Triggered, this, &AWoE_Character::OnZoom);
        }

        if (ToggleCameraModeAction) {
            // Started - called ONCE on press (not while held).
            EnhancedInput->BindAction(ToggleCameraModeAction, ETriggerEvent::Started, this, &AWoE_Character::OnToggleCameraMode);
        }
    }

}


// ================================================================
// INPUT HANDLERS
// ================================================================
void AWoE_Character::OnMove(const FInputActionValue& Value)
{
    // Get 2D vector from input.
    // For WASD: X = forward/back (W/S), Y = left/right (A/D).
    const FVector2D MovementVector = Value.Get<FVector2D>();

    if (Controller == nullptr) return;
    
    const FRotator Rotation = Controller->GetControlRotation(); // Get where camera is looking (controller rotation).
    const FRotator YawRotation(0, Rotation.Yaw, 0); // Take only Yaw (left/right turn), ignore pitch.

    // Calculate "forward" and "right" directions relative to camera.
    // FRotationMatrix - rotation matrix (transform math).
    // GetUnitAxis(EAxis::X) - unit vector "forward" for this rotation.
    const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
    const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

    // AddMovementInput - standard ACharacter function.
    // It passes direction to CharacterMovementComponent,
    // which handles physics, speed, gravity, etc.
    // Second parameter - scale (1.0 = full speed).
    AddMovementInput(ForwardDirection, MovementVector.X);
    AddMovementInput(RightDirection, MovementVector.Y);
}

void AWoE_Character::OnLook(const FInputActionValue& Value)
{
    // Camera rotation with mouse.
    const FVector2D LookAxisVector = Value.Get<FVector2D>();

    if (Controller == nullptr) return;

    if (CurrentCameraMode == EWoE_CameraMode::FirstPerson)
    {
        // In first person mode: mouse rotates camera freely.
        AddControllerYawInput(LookAxisVector.X);    // Left/right
        AddControllerPitchInput(LookAxisVector.Y);   // Up/down
    }
    else // Exploration
    {
        // In Exploration: mouse rotates camera around character
        // (only when right mouse button held - can be configured
        // in Input Action, but for MVP we always rotate).
        AddControllerYawInput(LookAxisVector.X);
        // Don't change Pitch in Exploration - angle is fixed.
    }
}


void AWoE_Character::OnZoom(const FInputActionValue& Value)
{
    // Mouse wheel - zoom in/out.
    const float ZoomValue = Value.Get<float>();

    // Change target arm length.
    CurrentArmLength -= ZoomValue * ZoomSpeed;
    // Minus because "scroll up" = zoom in (decrease length).

    // FMath::Clamp - limits value to [Min, Max] range.
    CurrentArmLength = FMath::Clamp(CurrentArmLength, MinArmLength, MaxArmLength);

    // If zoomed in very close - could auto-switch to First Person.
    if (CurrentArmLength <= MinArmLength + 10.0f)
    {
        // Can uncomment for automatic switch:
        // ApplyCameraMode(EWoE_CameraMode::FirstPerson);
    }
}

void AWoE_Character::OnToggleCameraMode(const FInputActionValue& Value)
{
    // Toggle camera mode (V key).
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
// CAMERA - update every frame
// ================================================================
void AWoE_Character::UpdateCamera(float DeltaTime)
{
    if (!CameraBoom) return;

    if (CurrentCameraMode == EWoE_CameraMode::Exploration)
    {
        // Smoothly interpolate arm length to target.
        // FInterpTo - smooth transition from current to target value.
        // InterpSpeed - higher = faster.
        const float NewArmLength = FMath::FInterpTo(
            CameraBoom->TargetArmLength,    // Current value
            CurrentArmLength,              // Target value
            DeltaTime,                     // Frame time
            CameraInterpSpeed              // Speed
        );
        CameraBoom->TargetArmLength = NewArmLength;

        // Fix pitch angle for Exploration.
        // Get current controller rotation.
        FRotator ControlRot = Controller ? Controller->GetControlRotation() : FRotator::ZeroRotator;

        // Smoothly move pitch toward ExplorationPitch.
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
        // In First Person arm = 0 (camera inside head).
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
// CAMERA - apply mode
// ================================================================
void AWoE_Character::ApplyCameraMode(EWoE_CameraMode NewMode)
{
    CurrentCameraMode = NewMode;

    switch (NewMode)
    {
    case EWoE_CameraMode::Exploration:
        // Character rotates toward movement direction.
        GetCharacterMovement()->bOrientRotationToMovement = true;
        bUseControllerRotationYaw = false;

        // Restore distance (if was in First Person).
        if (CurrentArmLength < MinArmLength + 50.0f)
        {
            CurrentArmLength = 400.0f; // Default distance
        }

        UE_LOG(LogTemp, Log, TEXT("Camera: Exploration mode"));
        break;

    case EWoE_CameraMode::FirstPerson:
        // In First Person character looks where camera looks.
        GetCharacterMovement()->bOrientRotationToMovement = false;
        bUseControllerRotationYaw = true;

        CurrentArmLength = 0.0f;

        UE_LOG(LogTemp, Log, TEXT("Camera: First Person mode"));
        break;
    }
}
