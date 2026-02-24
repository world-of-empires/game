// Character with hybrid camera system:
//   Exploration - top-down view (CameraBoom + FollowCamera)
//   FirstPerson - eye-level view (FirstPersonCamera on CapsuleComponent)
//
// Two-mesh architecture for FP:
//   GetMesh()       - full body, hidden from owner in FP, casts complete shadow
//   FirstPersonMesh - body copy visible only to owner, head/neck hidden, no shadow

#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "WoE_Character.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

UENUM(BlueprintType)
enum class EWoE_CameraMode : uint8
{
	Exploration    UMETA(DisplayName = "Exploration (Top-Down)"),
	FirstPerson    UMETA(DisplayName = "First Person"),
};

UCLASS()
class WORLDOFEMPIRES_API AWoE_Character : public ACharacter
{
	GENERATED_BODY()

public:
	AWoE_Character();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	virtual void BeginPlay() override;

	// ============================================================
	// COMPONENTS
	// ============================================================

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "WoE|Camera")
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "WoE|Camera")
	TObjectPtr<UCameraComponent> FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "WoE|Camera")
	TObjectPtr<UCameraComponent> FirstPersonCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "WoE|Camera")
	TObjectPtr<USkeletalMeshComponent> FirstPersonMesh;

	// ============================================================
	// CAMERA - General
	// ============================================================

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WoE|Camera")
	EWoE_CameraMode CurrentCameraMode;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WoE|Camera",
		meta = (ClampMin = "1.0", ClampMax = "30.0"))
	float CameraInterpSpeed;

	// ============================================================
	// CAMERA - Exploration
	// ============================================================

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WoE|Camera|Exploration")
	float CurrentArmLength;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WoE|Camera|Exploration")
	float MinArmLength;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WoE|Camera|Exploration")
	float MaxArmLength;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WoE|Camera|Exploration",
		meta = (ClampMin = "-89", ClampMax = "0"))
	float ExplorationPitch;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "WoE|Camera|Exploration")
	float DesiredYaw;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "WoE|Camera|Exploration")
	float DesiredPitch;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WoE|Camera|Exploration",
		meta = (ClampMin = "0.1", ClampMax = "5.0"))
	float ExplorationYawSensitivity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WoE|Camera|Exploration")
	float ZoomSpeed;

	// Spring arm pivot height above capsule center.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WoE|Camera|Exploration",
		meta = (ClampMin = "0.0", ClampMax = "200.0"))
	float ExplorationBoomHeight;

	// ============================================================
	// CAMERA - First Person
	// ============================================================

	// FP camera Z offset above capsule center. 60 ≈ eye level for default capsule.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WoE|Camera|FirstPerson",
		meta = (ClampMin = "-50.0", ClampMax = "200.0"))
	float FirstPersonEyeHeight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WoE|Camera|FirstPerson",
		meta = (ClampMin = "0.1", ClampMax = "5.0"))
	float FirstPersonLookSensitivity;

	// Separate FOV for FP mesh rendering (arms). Lower = arms appear larger.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WoE|Camera|FirstPerson",
		meta = (ClampMin = "40", ClampMax = "120"))
	float FirstPersonFOV;

	// Scale for FP mesh rendering. <1 pulls arms back to prevent wall clipping.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WoE|Camera|FirstPerson",
		meta = (ClampMin = "0.1", ClampMax = "1.5"))
	float FirstPersonScale;

	// ============================================================
	// MOVEMENT
	// ============================================================

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WoE|Movement",
		meta = (ClampMin = "100", ClampMax = "2000"))
	float RunSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WoE|Movement",
		meta = (ClampMin = "50", ClampMax = "500"))
	float WalkSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "WoE|Movement")
	bool bIsWalking;

	// ============================================================
	// INPUT
	// ============================================================

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WoE|Input")
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WoE|Input")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WoE|Input")
	TObjectPtr<UInputAction> LookAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WoE|Input")
	TObjectPtr<UInputAction> ZoomAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WoE|Input")
	TObjectPtr<UInputAction> ToggleCameraModeAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WoE|Input")
	TObjectPtr<UInputAction> JumpAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WoE|Input")
	TObjectPtr<UInputAction> WalkAction;

	// ============================================================
	// INPUT HANDLERS
	// ============================================================

	void OnMove(const FInputActionValue& Value);
	void OnLook(const FInputActionValue& Value);
	void OnZoom(const FInputActionValue& Value);
	void OnToggleCameraMode(const FInputActionValue& Value);
	void OnJumpStarted(const FInputActionValue& Value);
	void OnJumpCompleted(const FInputActionValue& Value);
	void OnWalkStarted(const FInputActionValue& Value);
	void OnWalkCompleted(const FInputActionValue& Value);

	// ============================================================
	// CAMERA - internal
	// ============================================================

	void UpdateCamera(float DeltaTime);
	void ApplyCameraMode(EWoE_CameraMode NewMode);

	bool bCameraInitialized;
};
