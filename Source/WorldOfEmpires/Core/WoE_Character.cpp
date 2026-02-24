// Hybrid camera: Exploration (top-down) + FirstPerson (eye-level).
// Two-mesh FP: main body for shadow, FP mesh for what the player sees.
// Click-to-move: LMB click in Exploration traces ground, character walks there.
// RMB hold: camera rotation in Exploration (cursor hidden, position restored).

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

	// ---- Spring Arm ----
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetRelativeLocation(FVector(0.f, 0.f, 85.f));
	CameraBoom->TargetArmLength = 1200.0f;
	CameraBoom->SetAbsolute(false, true, false);
	CameraBoom->bUsePawnControlRotation = false;
	CameraBoom->bDoCollisionTest = true;
	CameraBoom->bEnableCameraLag = true;
	CameraBoom->CameraLagSpeed = 8.0f;

	// ---- Follow Camera ----
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	// ---- First Person Mesh ----
	FirstPersonMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FirstPersonMesh"));
	FirstPersonMesh->SetupAttachment(GetMesh());
	FirstPersonMesh->SetOnlyOwnerSee(true);
	FirstPersonMesh->CastShadow = false;
	FirstPersonMesh->SetCollisionProfileName(FName("NoCollision"));
	FirstPersonMesh->SetVisibility(false);
	FirstPersonMesh->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::FirstPerson;

	// ---- First Person Camera ----
	FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCamera->SetupAttachment(GetCapsuleComponent());
	FirstPersonCamera->SetRelativeLocation(FVector(30.f, 0.f, 70.f));
	FirstPersonCamera->SetRelativeRotation(FRotator::ZeroRotator);
	FirstPersonCamera->bUsePawnControlRotation = true;
	FirstPersonCamera->SetAutoActivate(false);
	FirstPersonCamera->bEnableFirstPersonFieldOfView = true;
	FirstPersonCamera->FirstPersonFieldOfView = 70.0f;
	FirstPersonCamera->bEnableFirstPersonScale = true;
	FirstPersonCamera->FirstPersonScale = 0.6f;

	// ---- Main body mesh ----
	GetMesh()->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::WorldSpaceRepresentation;
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

	// ---- Click-to-move ----
	bIsClickMoving = false;
	bIsLMBHeld = false;
	ClickMoveDestination = FVector::ZeroVector;
	ClickMoveAcceptanceRadius = 50.0f;
	ClickHoldThreshold = 0.2f;
	LMBPressTime = 0.0f;

	// ---- Exploration ----
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
	bIsRotatingCamera = false;
	PreRotateMouseX = 0.0f;
	PreRotateMouseY = 0.0f;

	// ---- First Person ----
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

	if (FirstPersonMesh && GetMesh())
	{
		FirstPersonMesh->SetLeaderPoseComponent(GetMesh());
		FirstPersonMesh->HideBoneByName(FName("head"), EPhysBodyOp::PBO_None);
		FirstPersonMesh->HideBoneByName(FName("neck_01"), EPhysBodyOp::PBO_None);
		FirstPersonMesh->HideBoneByName(FName("neck_02"), EPhysBodyOp::PBO_None);
	}

	GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
	bIsWalking = false;
	bIsClickMoving = false;
	bIsLMBHeld = false;
	bIsRotatingCamera = false;
	bCameraInitialized = false;

	if (APlayerController* PC = Cast<APlayerController>(Controller))
	{
		ApplyCursorSettings(CurrentCameraMode);

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
	UpdateClickToMove(DeltaTime);
	Super::Tick(DeltaTime);
}

// ================================================================
// INPUT BINDING
// ================================================================
void AWoE_Character::SetupPlayerInputComponent(UInputComponent* PIC)
{
	Super::SetupPlayerInputComponent(PIC);
	UEnhancedInputComponent* EI = CastChecked<UEnhancedInputComponent>(PIC);

	if (MoveAction)             EI->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AWoE_Character::OnMove);
	if (LookAction)             EI->BindAction(LookAction, ETriggerEvent::Triggered, this, &AWoE_Character::OnLook);
	if (ZoomAction)             EI->BindAction(ZoomAction, ETriggerEvent::Triggered, this, &AWoE_Character::OnZoom);
	if (ToggleCameraModeAction) EI->BindAction(ToggleCameraModeAction, ETriggerEvent::Started, this, &AWoE_Character::OnToggleCameraMode);

	if (JumpAction)
	{
		EI->BindAction(JumpAction, ETriggerEvent::Started, this, &AWoE_Character::OnJumpStarted);
		EI->BindAction(JumpAction, ETriggerEvent::Completed, this, &AWoE_Character::OnJumpCompleted);
	}
	if (WalkAction)
	{
		EI->BindAction(WalkAction, ETriggerEvent::Started, this, &AWoE_Character::OnWalkStarted);
		EI->BindAction(WalkAction, ETriggerEvent::Completed, this, &AWoE_Character::OnWalkCompleted);
	}

	if (ClickToMoveAction)
	{
		EI->BindAction(ClickToMoveAction, ETriggerEvent::Started, this, &AWoE_Character::OnClickToMoveStarted);
		EI->BindAction(ClickToMoveAction, ETriggerEvent::Triggered, this, &AWoE_Character::OnClickToMoveTriggered);
		EI->BindAction(ClickToMoveAction, ETriggerEvent::Completed, this, &AWoE_Character::OnClickToMoveReleased);
	}

	if (RotateCameraAction)
	{
		EI->BindAction(RotateCameraAction, ETriggerEvent::Started, this, &AWoE_Character::OnRotateCameraStarted);
		EI->BindAction(RotateCameraAction, ETriggerEvent::Completed, this, &AWoE_Character::OnRotateCameraCompleted);
	}
}

// ================================================================
// MOVEMENT (WASD)
// ================================================================
void AWoE_Character::OnMove(const FInputActionValue& Value)
{
	const FVector2D Axis = Value.Get<FVector2D>();
	if (!Controller) return;

	const bool bHasInput = (FMath::Abs(Axis.X) > 0.1f || FMath::Abs(Axis.Y) > 0.1f);
	if (!bHasInput) return;

	// -------------------------------------------------------
	// LMB held → mouse-follow has full priority, ignore WASD.
	// -------------------------------------------------------
	if (bIsLMBHeld) return;

	// -------------------------------------------------------
	// Short-click run-to-point active → WASD cancels it.
	// -------------------------------------------------------
	if (bIsClickMoving)
	{
		CancelClickToMove();
	}

	// -------------------------------------------------------
	// Normal WASD movement (camera-relative).
	// -------------------------------------------------------
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
		if (!bIsRotatingCamera) return;

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
void AWoE_Character::OnJumpStarted(const FInputActionValue&) { Jump(); }
void AWoE_Character::OnJumpCompleted(const FInputActionValue&) { StopJumping(); }

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
// CLICK-TO-MOVE: LMB PRESSED
// ================================================================
void AWoE_Character::OnClickToMoveStarted(const FInputActionValue& Value)
{
	if (CurrentCameraMode != EWoE_CameraMode::Exploration) return;
	if (bIsRotatingCamera) return;

	bIsLMBHeld = true;
	LMBPressTime = GetWorld()->GetTimeSeconds();

	if (TraceClickDestination())
	{
		bIsClickMoving = true;
	}
}

// ================================================================
// CLICK-TO-MOVE: LMB HELD (every frame while pressed)
// ================================================================
void AWoE_Character::OnClickToMoveTriggered(const FInputActionValue& Value)
{
	if (!bIsLMBHeld) return;
	if (CurrentCameraMode != EWoE_CameraMode::Exploration) return;
	if (bIsRotatingCamera) return;

	// Update destination to follow cursor every frame.
	if (TraceClickDestination())
	{
		bIsClickMoving = true;
	}
}

// ================================================================
// CLICK-TO-MOVE: LMB RELEASED
// ================================================================
void AWoE_Character::OnClickToMoveReleased(const FInputActionValue& Value)
{
	const bool WasHeld = bIsLMBHeld;
	bIsLMBHeld = false;

	if (!WasHeld) return;

	const float Duration = GetWorld()->GetTimeSeconds() - LMBPressTime;
	if (Duration >= ClickHoldThreshold)
	{
		// Long hold → stop now.
		CancelClickToMove();
	}
	// Short click → bIsClickMoving stays true,
	// character continues to ClickMoveDestination.
}

// ================================================================
// CLICK-TO-MOVE: UPDATE (every Tick)
// ================================================================
void AWoE_Character::UpdateClickToMove(float DeltaTime)
{
	if (!bIsClickMoving) return;

	if (CurrentCameraMode != EWoE_CameraMode::Exploration)
	{
		CancelClickToMove();
		return;
	}

	const FVector CurrentLoc = GetActorLocation();
	FVector ToTarget = ClickMoveDestination - CurrentLoc;
	ToTarget.Z = 0.0f;

	const float Dist2D = ToTarget.Size();
	if (Dist2D <= ClickMoveAcceptanceRadius)
	{
		// Arrived at point.
		if (!bIsLMBHeld)
		{
			// Short-click: done.
			CancelClickToMove();
		}
		// LMB held: wait, cursor may move → next Triggered updates destination.
		return;
	}

	ToTarget.Normalize();
	AddMovementInput(ToTarget, 1.0f);
}

// ================================================================
// CLICK-TO-MOVE: TRACE HELPER
// ================================================================
bool AWoE_Character::TraceClickDestination()
{
	APlayerController* PC = Cast<APlayerController>(Controller);
	if (!PC) return false;

	FHitResult Hit;
	if (PC->GetHitResultUnderCursor(ECC_Visibility, true, Hit))
	{
		ClickMoveDestination = Hit.ImpactPoint;
		return true;
	}
	return false;
}

// ================================================================
// CLICK-TO-MOVE: CANCEL
// ================================================================
void AWoE_Character::CancelClickToMove()
{
	bIsClickMoving = false;
}

// ================================================================
// ROTATE CAMERA (RMB)
// ================================================================
void AWoE_Character::OnRotateCameraStarted(const FInputActionValue& Value)
{
	if (CurrentCameraMode != EWoE_CameraMode::Exploration) return;
	bIsRotatingCamera = true;

	if (APlayerController* PC = Cast<APlayerController>(Controller))
	{
		PC->GetMousePosition(PreRotateMouseX, PreRotateMouseY);
		PC->bShowMouseCursor = false;
	}
}

void AWoE_Character::OnRotateCameraCompleted(const FInputActionValue& Value)
{
	bIsRotatingCamera = false;

	if (CurrentCameraMode == EWoE_CameraMode::Exploration)
	{
		if (APlayerController* PC = Cast<APlayerController>(Controller))
		{
			PC->bShowMouseCursor = true;
			PC->SetMouseLocation(
				static_cast<int>(PreRotateMouseX),
				static_cast<int>(PreRotateMouseY));
		}
	}
}

// ================================================================
// CAMERA UPDATE
// ================================================================
void AWoE_Character::UpdateCamera(float DeltaTime)
{
	if (!CameraBoom) return;

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

		if (GetMesh())       GetMesh()->SetOwnerNoSee(false);
		if (FirstPersonMesh) FirstPersonMesh->SetVisibility(false);

		bIsRotatingCamera = false;
		ApplyCursorSettings(NewMode);
		break;
	}

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

		if (GetMesh()) GetMesh()->SetOwnerNoSee(true);
		if (FirstPersonMesh) FirstPersonMesh->SetVisibility(true);

		CancelClickToMove();
		bIsLMBHeld = false;
		bIsRotatingCamera = false;
		ApplyCursorSettings(NewMode);
		break;
	}
	}
}

// ================================================================
// CURSOR / INPUT MODE
// ================================================================
void AWoE_Character::ApplyCursorSettings(EWoE_CameraMode Mode)
{
	APlayerController* PC = Cast<APlayerController>(Controller);
	if (!PC) return;

	if (Mode == EWoE_CameraMode::Exploration)
	{
		PC->bShowMouseCursor = true;
		FInputModeGameAndUI InputMode;
		InputMode.SetHideCursorDuringCapture(false);
		PC->SetInputMode(InputMode);
	}
	else
	{
		PC->bShowMouseCursor = false;
		PC->SetInputMode(FInputModeGameOnly());
	}
}