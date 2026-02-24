// WoE_Character.cpp
// Hybrid camera: Exploration (top-down) + FirstPerson (eye-level).
// Two-mesh FP: main body for shadow, FP mesh for what the player sees.

#include "WoE_Character.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
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

	// ---- Spring Arm (Exploration top-down) ----
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetRelativeLocation(FVector(0.f, 0.f, 85.f));
	CameraBoom->TargetArmLength = 1200.0f;
	CameraBoom->SetAbsolute(false, true, false);   // absolute rotation
	CameraBoom->bUsePawnControlRotation = false;
	CameraBoom->bDoCollisionTest = true;
	CameraBoom->bEnableCameraLag = true;
	CameraBoom->CameraLagSpeed = 8.0f;

	// ---- Follow Camera (end of spring arm) ----
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	// ---- First Person Mesh (what the owner sees in FP) ----
	FirstPersonMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FirstPersonMesh"));
	FirstPersonMesh->SetupAttachment(GetMesh());
	FirstPersonMesh->SetOnlyOwnerSee(true);
	FirstPersonMesh->CastShadow = false;
	FirstPersonMesh->SetCollisionProfileName(FName("NoCollision"));
	FirstPersonMesh->SetVisibility(false);
	// UE5 FP rendering: separate near-clip/FOV so arms don't clip through walls.
	FirstPersonMesh->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::FirstPerson;

	// ---- First Person Camera (fixed to capsule) ----
	FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCamera->SetupAttachment(GetCapsuleComponent());
	FirstPersonCamera->SetRelativeLocation(FVector(30.f, 0.f, 70.f));
	FirstPersonCamera->SetRelativeRotation(FRotator::ZeroRotator);
	FirstPersonCamera->bUsePawnControlRotation = true;
	FirstPersonCamera->SetAutoActivate(false);
	// Enable FP rendering pipeline on this camera.
	FirstPersonCamera->bEnableFirstPersonFieldOfView = true;
	FirstPersonCamera->FirstPersonFieldOfView = 70.0f;
	FirstPersonCamera->bEnableFirstPersonScale = true;
	FirstPersonCamera->FirstPersonScale = 0.6f;

	// ---- Main body mesh (shadow caster, visible to others) ----
	// WorldSpaceRepresentation = rendered for shadows/reflections, not over FP arms.
	GetMesh()->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::WorldSpaceRepresentation;
	// Always evaluate animation even when OwnerNoSee hides rendering.
	// Without this, LeaderPose stops copying and FP mesh freezes.
	GetMesh()->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;

	// ---- Character rotation ----
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// ---- Movement ----
	RunSpeed = 600.0f;
	WalkSpeed = 300.0f;
	bIsWalking = false;
	GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
	GetCharacterMovement()->JumpZVelocity = 500.0f;
	GetCharacterMovement()->AirControl = 0.3f;
	JumpMaxCount = 1;

	// ---- Defaults: Exploration ----
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
	ExplorationBoomHeight = 85.0f;

	// ---- Defaults: First Person ----
	FirstPersonEyeHeight = 70.0f;
	FirstPersonLookSensitivity = 1.0f;
	FirstPersonFOV = 70.0f;
	FirstPersonScale = 0.6f;
	bCameraInitialized = false;
}

// ================================================================
// BEGIN PLAY
// ================================================================
void AWoE_Character::BeginPlay()
{
	Super::BeginPlay();

	// Force runtime values that Blueprint may have overridden.
	GetMesh()->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;

	if (CameraBoom)
	{
		CameraBoom->bUsePawnControlRotation = false;
		CameraBoom->SetAbsolute(false, true, false);
		CameraBoom->bDoCollisionTest = true;
		CameraBoom->bEnableCameraLag = true;
		CameraBoom->CameraLagSpeed = 8.0f;
		CameraBoom->TargetArmLength = CurrentArmLength;
		CameraBoom->SetRelativeLocation(FVector(0.f, 0.f, ExplorationBoomHeight));
	}

	DesiredYaw = GetActorRotation().Yaw;
	DesiredPitch = ExplorationPitch;

	if (CameraBoom)
	{
		CameraBoom->SetWorldRotation(FRotator(DesiredPitch, DesiredYaw, 0.0f));
	}

	if (FirstPersonCamera)
	{
		// Auto-calculate eye height if not manually overridden:
		// CapsuleHalfHeight - 10 puts camera near the top of the capsule (eye level).
		const float CapsuleHH = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
		if (FMath::IsNearlyEqual(FirstPersonEyeHeight, 60.0f))
		{
			FirstPersonEyeHeight = CapsuleHH - 10.0f;
		}

		FirstPersonCamera->SetRelativeLocation(FVector(30.f, 0.f, FirstPersonEyeHeight));
		FirstPersonCamera->SetRelativeRotation(FRotator::ZeroRotator);
		FirstPersonCamera->bEnableFirstPersonFieldOfView = true;
		FirstPersonCamera->FirstPersonFieldOfView = FirstPersonFOV;
		FirstPersonCamera->bEnableFirstPersonScale = true;
		FirstPersonCamera->FirstPersonScale = FirstPersonScale;

		UE_LOG(LogTemp, Log, TEXT("WoE FP Camera: CapsuleHH=%.1f  EyeHeight=%.1f  WorldZ=%.1f"),
			CapsuleHH, FirstPersonEyeHeight,
			GetActorLocation().Z + FirstPersonEyeHeight);
	}

	// FP mesh copies pose from main body. Hide head/neck so camera doesn't clip.
	if (FirstPersonMesh && GetMesh())
	{
		FirstPersonMesh->SetLeaderPoseComponent(GetMesh());
		FirstPersonMesh->HideBoneByName(FName("head"), EPhysBodyOp::PBO_None);
		FirstPersonMesh->HideBoneByName(FName("neck_01"), EPhysBodyOp::PBO_None);
		FirstPersonMesh->HideBoneByName(FName("neck_02"), EPhysBodyOp::PBO_None);
	}

	GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
	bIsWalking = false;
	bCameraInitialized = false;

	if (APlayerController* PC = Cast<APlayerController>(Controller))
	{
		PC->bShowMouseCursor = false;
		PC->SetInputMode(FInputModeGameOnly());

		if (UEnhancedInputLocalPlayerSubsystem* Sub =
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			if (DefaultMappingContext)
				Sub->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	ApplyCameraMode(CurrentCameraMode);
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
void AWoE_Character::SetupPlayerInputComponent(UInputComponent* PIC)
{
	Super::SetupPlayerInputComponent(PIC);
	UEnhancedInputComponent* EI = CastChecked<UEnhancedInputComponent>(PIC);

	if (MoveAction)             EI->BindAction(MoveAction,             ETriggerEvent::Triggered, this, &AWoE_Character::OnMove);
	if (LookAction)             EI->BindAction(LookAction,             ETriggerEvent::Triggered, this, &AWoE_Character::OnLook);
	if (ZoomAction)             EI->BindAction(ZoomAction,             ETriggerEvent::Triggered, this, &AWoE_Character::OnZoom);
	if (ToggleCameraModeAction) EI->BindAction(ToggleCameraModeAction, ETriggerEvent::Started,   this, &AWoE_Character::OnToggleCameraMode);
	if (JumpAction)
	{
		EI->BindAction(JumpAction, ETriggerEvent::Started,   this, &AWoE_Character::OnJumpStarted);
		EI->BindAction(JumpAction, ETriggerEvent::Completed, this, &AWoE_Character::OnJumpCompleted);
	}
	if (WalkAction)
	{
		EI->BindAction(WalkAction, ETriggerEvent::Started,   this, &AWoE_Character::OnWalkStarted);
		EI->BindAction(WalkAction, ETriggerEvent::Completed, this, &AWoE_Character::OnWalkCompleted);
	}
}

// ================================================================
// MOVEMENT
// ================================================================
void AWoE_Character::OnMove(const FInputActionValue& Value)
{
	const FVector2D Axis = Value.Get<FVector2D>();
	if (!Controller) return;

	const float Yaw = (CurrentCameraMode == EWoE_CameraMode::Exploration)
		? DesiredYaw
		: Controller->GetControlRotation().Yaw;

	const FRotator Rot(0.0f, Yaw, 0.0f);
	AddMovementInput(FRotationMatrix(Rot).GetUnitAxis(EAxis::X), Axis.Y);
	AddMovementInput(FRotationMatrix(Rot).GetUnitAxis(EAxis::Y), Axis.X);
}

// ================================================================
// LOOK
// ================================================================
void AWoE_Character::OnLook(const FInputActionValue& Value)
{
	const FVector2D Delta = Value.Get<FVector2D>();
	if (!Controller) return;
	if (FMath::Abs(Delta.X) > 200.0f || FMath::Abs(Delta.Y) > 200.0f) return;

	if (CurrentCameraMode == EWoE_CameraMode::FirstPerson)
	{
		AddControllerYawInput(Delta.X * FirstPersonLookSensitivity);
		AddControllerPitchInput(Delta.Y * FirstPersonLookSensitivity);
	}
	else
	{
		DesiredYaw += Delta.X * ExplorationYawSensitivity;
		DesiredPitch = FMath::Clamp(
			DesiredPitch + Delta.Y * ExplorationYawSensitivity, -89.0f, -5.0f);
	}
}

// ================================================================
// ZOOM
// ================================================================
void AWoE_Character::OnZoom(const FInputActionValue& Value)
{
	if (CurrentCameraMode != EWoE_CameraMode::Exploration) return;
	CurrentArmLength = FMath::Clamp(
		CurrentArmLength - Value.Get<float>() * ZoomSpeed, MinArmLength, MaxArmLength);
}

// ================================================================
// TOGGLE CAMERA
// ================================================================
void AWoE_Character::OnToggleCameraMode(const FInputActionValue& Value)
{
	ApplyCameraMode(CurrentCameraMode == EWoE_CameraMode::Exploration
		? EWoE_CameraMode::FirstPerson
		: EWoE_CameraMode::Exploration);
}

// ================================================================
// JUMP / WALK
// ================================================================
void AWoE_Character::OnJumpStarted(const FInputActionValue&)    { Jump(); }
void AWoE_Character::OnJumpCompleted(const FInputActionValue&)  { StopJumping(); }

void AWoE_Character::OnWalkStarted(const FInputActionValue&)
{
	bIsWalking = true;
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

void AWoE_Character::OnWalkCompleted(const FInputActionValue&)
{
	bIsWalking = false;
	GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
}

// ================================================================
// CAMERA UPDATE (every frame)
// ================================================================
void AWoE_Character::UpdateCamera(float DeltaTime)
{
	if (!CameraBoom) return;

	// First frame: snap instantly so there's no visible shift.
	if (!bCameraInitialized)
	{
		bCameraInitialized = true;
		if (CurrentCameraMode == EWoE_CameraMode::Exploration)
		{
			CameraBoom->TargetArmLength = CurrentArmLength;
			CameraBoom->SetWorldRotation(FRotator(DesiredPitch, DesiredYaw, 0.0f));
		}
		else
		{
			CameraBoom->TargetArmLength = 0.0f;
		}
		return;
	}

	if (CurrentCameraMode == EWoE_CameraMode::Exploration)
	{
		CameraBoom->TargetArmLength = FMath::FInterpTo(
			CameraBoom->TargetArmLength, CurrentArmLength, DeltaTime, CameraInterpSpeed);

		const float CurPitch = CameraBoom->GetComponentRotation().Pitch;
		const float NewPitch = FMath::FInterpTo(CurPitch, DesiredPitch, DeltaTime, CameraInterpSpeed);
		CameraBoom->SetWorldRotation(FRotator(NewPitch, DesiredYaw, 0.0f));
	}
	else
	{
		CameraBoom->TargetArmLength = FMath::FInterpTo(
			CameraBoom->TargetArmLength, 0.0f, DeltaTime, CameraInterpSpeed * 2.0f);
	}
}

// ================================================================
// APPLY CAMERA MODE
// ================================================================
void AWoE_Character::ApplyCameraMode(EWoE_CameraMode NewMode)
{
	CurrentCameraMode = NewMode;

	switch (NewMode)
	{
	// ── EXPLORATION (top-down) ──────────────────────────────────
	case EWoE_CameraMode::Exploration:
	{
		GetCharacterMovement()->bOrientRotationToMovement = true;
		bUseControllerRotationYaw = false;

		CameraBoom->SetAbsolute(false, true, false);
		CameraBoom->bUsePawnControlRotation = false;
		CameraBoom->bDoCollisionTest = true;
		CameraBoom->bEnableCameraLag = true;
		CameraBoom->CameraLagSpeed = 8.0f;
		CameraBoom->TargetOffset = FVector::ZeroVector;
		CameraBoom->SocketOffset = FVector::ZeroVector;
		CameraBoom->SetRelativeLocation(FVector(0.f, 0.f, ExplorationBoomHeight));

		if (Controller) DesiredYaw = Controller->GetControlRotation().Yaw;
		DesiredPitch = ExplorationPitch;
		if (CurrentArmLength < MinArmLength + 50.0f) CurrentArmLength = 1200.0f;

		if (FollowCamera)      FollowCamera->SetActive(true);
		if (FirstPersonCamera) FirstPersonCamera->SetActive(false);

		// Full body visible. FP mesh hidden.
		if (GetMesh())       GetMesh()->SetOwnerNoSee(false);
		if (FirstPersonMesh) FirstPersonMesh->SetVisibility(false);
		break;
	}

	// ── FIRST PERSON ────────────────────────────────────────────
	case EWoE_CameraMode::FirstPerson:
	{
		if (Controller)
			Controller->SetControlRotation(FRotator(0.0f, DesiredYaw, 0.0f));

		bUseControllerRotationYaw = true;
		GetCharacterMovement()->bOrientRotationToMovement = false;

		CameraBoom->SetAbsolute(false, false, false);
		CameraBoom->bUsePawnControlRotation = true;
		CameraBoom->bDoCollisionTest = false;
		CameraBoom->bEnableCameraLag = false;
		CameraBoom->TargetOffset = FVector::ZeroVector;
		CameraBoom->SocketOffset = FVector::ZeroVector;
		CurrentArmLength = 0.0f;
		CameraBoom->TargetArmLength = 0.0f;

		if (FollowCamera)      FollowCamera->SetActive(false);
		if (FirstPersonCamera) FirstPersonCamera->SetActive(true);

		// Main mesh: hidden from owner, still casts FULL shadow (light POV).
		if (GetMesh()) GetMesh()->SetOwnerNoSee(true);
		// FP mesh: owner sees body minus head/neck. No shadow (CastShadow=false).
		if (FirstPersonMesh) FirstPersonMesh->SetVisibility(true);
		break;
	}
	}
}
