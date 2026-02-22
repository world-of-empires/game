// Main character class for World of Empires.
// ACharacter provides: Capsule, SkeletalMesh, CharacterMovement.
// We add: hybrid camera (Exploration top-down + FirstPerson), input handling, mode switching.
// Toggle camera with key V.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "WoE_Character.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

// ================================================================
// Camera modes
// ================================================================

UENUM(BlueprintType)
enum class EWoE_CameraMode : uint8
{
	Exploration    UMETA(DisplayName = "Exploration (Top-Down)"),
	FirstPerson    UMETA(DisplayName = "First Person"),
};

// ================================================================
// Character
// ================================================================

UCLASS()
class WORLDOFEMPIRES_API AWoE_Character : public ACharacter
{
	GENERATED_BODY()

public:
	AWoE_Character();

	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(
		class UInputComponent* PlayerInputComponent) override;

protected:
	virtual void BeginPlay() override;

	// ================================================================
	// COMPONENTS
	// ================================================================

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "WoE|Camera")
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "WoE|Camera")
	TObjectPtr<UCameraComponent> FollowCamera;

	// ================================================================
	// CAMERA - common
	// ================================================================

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WoE|Camera")
	EWoE_CameraMode CurrentCameraMode;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WoE|Camera")
	float CameraInterpSpeed;

	// ================================================================
	// CAMERA - Exploration
	// ================================================================

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WoE|Camera")
	float CurrentArmLength;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WoE|Camera")
	float MinArmLength;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WoE|Camera")
	float MaxArmLength;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WoE|Camera",
		meta = (ClampMin = "-89", ClampMax = "0"))
	float ExplorationPitch;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "WoE|Camera")
	float DesiredYaw;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "WoE|Camera")
	float DesiredPitch;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WoE|Camera",
		meta = (ClampMin = "0.1", ClampMax = "5.0"))
	float ExplorationYawSensitivity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WoE|Camera")
	float ZoomSpeed;

	// ================================================================
	// CAMERA - First Person
	// ================================================================

	// Camera height above capsule root (eye level). Capsule half-height ~88, eyes ~70.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WoE|Camera|FirstPerson",
		meta = (ClampMin = "0.0", ClampMax = "200.0"))
	float FirstPersonCameraHeight;

	// Hide own mesh in FP mode. true = no body clipping from inside; other players still see your model.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WoE|Camera|FirstPerson")
	bool bHideMeshInFirstPerson;

	// Mouse sensitivity in FP. Exploration has its own; FP has separate sensitivity for head look.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WoE|Camera|FirstPerson",
		meta = (ClampMin = "0.1", ClampMax = "5.0"))
	float FirstPersonLookSensitivity;

	// ================================================================
	// INPUT - references to Input Action assets
	// ================================================================

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

	// ================================================================
	// INPUT - handler functions
	// ================================================================

	void OnMove(const FInputActionValue& Value);
	void OnLook(const FInputActionValue& Value);
	void OnZoom(const FInputActionValue& Value);
	void OnToggleCameraMode(const FInputActionValue& Value);

	// ================================================================
	// CAMERA - internal
	// ================================================================

	void UpdateCamera(float DeltaTime);
	void ApplyCameraMode(EWoE_CameraMode NewMode);
};
