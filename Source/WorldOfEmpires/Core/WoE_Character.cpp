// WoE_Character.cpp
// Path: Source/WorldOfEmpires/Core/WoE_Character.cpp

#include "WoE_Character.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"

// ================================================================
// CONSTRUCTOR
// ================================================================
AWoE_Character::AWoE_Character()
{
	PrimaryActorTick.bCanEverTick = true;

	// ---- Spring Arm ----
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);

	// Raise boom attachment point to eye level. Same for both modes; in Exploration
	// (camera far) 70 units up is barely noticeable; in FirstPerson it defines view height.
	CameraBoom->SetRelativeLocation(FVector(0.f, 0.f, 70.f));

	CameraBoom->bUsePawnControlRotation = false;
	CameraBoom->SetAbsolute(false, true, false);
	CameraBoom->TargetArmLength = 1200.0f;

	// Enable collision test. Boom shortens automatically if obstacle between camera and character.
	CameraBoom->bDoCollisionTest = true;

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

	// ---- Default values - Exploration ----
	CurrentCameraMode = EWoE_CameraMode::Exploration;
	CurrentArmLength = 1200.0f;
	MinArmLength = 300.0f;
	MaxArmLength = 2500.0f;
	ExplorationPitch = -55.0f;
	DesiredYaw = 0.0f;
	DesiredPitch = -55.0f;
	ExplorationYawSensitivity = 1.0f;
	ZoomSpeed = 80.0f;
	CameraInterpSpeed = 8.0f;

	// ---- Default values - First Person ----
	FirstPersonCameraHeight = 70.0f;
	bHideMeshInFirstPerson = true;
	FirstPersonLookSensitivity = 1.0f;
}

// ================================================================
// BEGIN PLAY
// ================================================================
void AWoE_Character::BeginPlay()
{
	Super::BeginPlay();

	// Force component settings at runtime. Blueprint may have saved old values;
	// this guarantees correct initial state.
	if (CameraBoom)
	{
		CameraBoom->bUsePawnControlRotation = false;
		CameraBoom->SetAbsolute(false, true, false);
		CameraBoom->bDoCollisionTest = true;
		CameraBoom->bEnableCameraLag = true;
		CameraBoom->CameraLagSpeed = 8.0f;
		CameraBoom->TargetArmLength = CurrentArmLength;
		CameraBoom->SetRelativeLocation(FVector(0.f, 0.f, FirstPersonCameraHeight));
	}

	DesiredYaw = GetActorRotation().Yaw;
	DesiredPitch = ExplorationPitch;

	if (CameraBoom)
	{
		CameraBoom->SetWorldRotation(FRotator(DesiredPitch, DesiredYaw, 0.0f));
	}

	// Hide cursor and capture mouse. Standard for 3D action; when opening inventory
	// switch to FInputModeGameAndUI and show cursor.
	if (APlayerController* PC = Cast<APlayerController>(Controller))
	{
		PC->bShowMouseCursor = false;
		PC->SetInputMode(FInputModeGameOnly());

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
// INPUT: MOVEMENT (WASD)
// ================================================================
void AWoE_Character::OnMove(const FInputActionValue& Value)
{
	const FVector2D MovementVector = Value.Get<FVector2D>();
	if (Controller == nullptr) return;

	// Forward direction depends on camera mode. Exploration: forward = camera direction (DesiredYaw).
	// FirstPerson: forward = player look direction (Controller Rotation).
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

// ================================================================
// INPUT: LOOK (MOUSE)
// ================================================================
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
		// Use FirstPersonLookSensitivity. AddControllerYawInput/PitchInput add to Controller Rotation.
		// Pitch is clamped by PlayerCameraManager (+/-89.9) - camera won't flip.
		AddControllerYawInput(LookAxisVector.X * FirstPersonLookSensitivity);
		AddControllerPitchInput(LookAxisVector.Y * FirstPersonLookSensitivity);
	}
	else
	{
		// Exploration: mouse rotates camera AROUND character.
		DesiredYaw += LookAxisVector.X * ExplorationYawSensitivity;
		DesiredPitch += LookAxisVector.Y * ExplorationYawSensitivity;
		DesiredPitch = FMath::Clamp(DesiredPitch, -89.0f, -5.0f);
	}
}

// ================================================================
// INPUT: ZOOM (MOUSE WHEEL)
// ================================================================
void AWoE_Character::OnZoom(const FInputActionValue& Value)
{
	if (CurrentCameraMode != EWoE_CameraMode::Exploration) return;

	const float ZoomValue = Value.Get<float>();
	CurrentArmLength -= ZoomValue * ZoomSpeed;
	CurrentArmLength = FMath::Clamp(CurrentArmLength, MinArmLength, MaxArmLength);
}

// ================================================================
// INPUT: TOGGLE CAMERA (V)
// ================================================================
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
// CAMERA - per-frame update
// ================================================================
void AWoE_Character::UpdateCamera(float DeltaTime)
{
	if (!CameraBoom) return;

	if (CurrentCameraMode == EWoE_CameraMode::Exploration)
	{
		// Smooth zoom.
		CameraBoom->TargetArmLength = FMath::FInterpTo(
			CameraBoom->TargetArmLength, CurrentArmLength, DeltaTime, CameraInterpSpeed);

		// Set boom world rotation directly - independent of controller rotation.
		// SetAbsolute(rotation=true) allows direct world rotation.
		const FRotator CurrentRot = CameraBoom->GetComponentRotation();
		const float NewPitch = FMath::FInterpTo(
			CurrentRot.Pitch, DesiredPitch, DeltaTime, CameraInterpSpeed);

		// Yaw applied instantly for 1:1 mouse response. Pitch interpolated for smooth mode switch.
		CameraBoom->SetWorldRotation(FRotator(NewPitch, DesiredYaw, 0.0f));
	}
	else if (CurrentCameraMode == EWoE_CameraMode::FirstPerson)
	{
		// Smoothly reduce arm length to 0. Interpolation x2 for fast FP transition.
		CameraBoom->TargetArmLength = FMath::FInterpTo(
			CameraBoom->TargetArmLength, 0.0f, DeltaTime, CameraInterpSpeed * 2.0f);

		// Rotation fully controlled by bUsePawnControlRotation + AddControllerYaw/PitchInput from OnLook.
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
	// ------------------------------------------------------------------------
	//  EXPLORATION
	// ------------------------------------------------------------------------
	case EWoE_CameraMode::Exploration:
	{
		// --- Character ---
		// Body rotates toward movement direction (like TPS).
		GetCharacterMovement()->bOrientRotationToMovement = true;
		bUseControllerRotationYaw = false;

		// --- Boom ---
		CameraBoom->SetAbsolute(false, true, false);
		CameraBoom->bUsePawnControlRotation = false;
		CameraBoom->bDoCollisionTest = true;
		CameraBoom->bEnableCameraLag = true;
		CameraBoom->CameraLagSpeed = 8.0f;
		CameraBoom->TargetOffset = FVector::ZeroVector;
		CameraBoom->SocketOffset = FVector::ZeroVector;
		CameraBoom->SetRelativeLocation(FVector(0.f, 0.f, FirstPersonCameraHeight));

		// --- Sync angles ---
		// Take current Yaw from Controller Rotation (from FP mode) into DesiredYaw for smooth transition.
		if (Controller)
		{
			DesiredYaw = Controller->GetControlRotation().Yaw;
		}
		DesiredPitch = ExplorationPitch;

		if (CurrentArmLength < MinArmLength + 50.0f)
		{
			CurrentArmLength = 1200.0f;
		}

		if (GetMesh())
		{
			GetMesh()->SetOwnerNoSee(false);
		}

		UE_LOG(LogTemp, Log, TEXT("Camera -> Exploration"));
		break;
	}

	// ------------------------------------------------------------------------
	//  FIRST PERSON
	// ------------------------------------------------------------------------
	case EWoE_CameraMode::FirstPerson:
	{
		// --- Sync angles (BEFORE changing flags!) ---
		// Transfer current camera angles (DesiredYaw + DesiredPitch) to Controller Rotation.
		// Previously pitch was forced to 0 - camera jerked. Now transition is smooth.
		if (Controller)
		{
			Controller->SetControlRotation(FRotator(DesiredPitch, DesiredYaw, 0.0f));
		}

		// --- Character ---
		bUseControllerRotationYaw = true;
		GetCharacterMovement()->bOrientRotationToMovement = false;

		// --- Boom ---
		CameraBoom->SetAbsolute(false, false, false);
		CameraBoom->bUsePawnControlRotation = true;
		CameraBoom->bDoCollisionTest = false;
		CameraBoom->bEnableCameraLag = false;
		CameraBoom->TargetOffset = FVector::ZeroVector;
		CameraBoom->SocketOffset = FVector::ZeroVector;
		CameraBoom->SetRelativeLocation(FVector(0.f, 0.f, FirstPersonCameraHeight));

		CurrentArmLength = 0.0f;

		// SetOwnerNoSee: mesh not rendered for owning Pawn. Other players still see your model.
		if (bHideMeshInFirstPerson && GetMesh())
		{
			GetMesh()->SetOwnerNoSee(true);
		}

		UE_LOG(LogTemp, Log, TEXT("Camera -> First Person"));
		break;
	}
	}
}
