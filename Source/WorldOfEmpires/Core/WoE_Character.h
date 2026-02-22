// Main character class for World of Empires.
// ACharacter already provides: Capsule, SkeletalMesh, CharacterMovement.
// We add: top-down camera (King's Bounty style), input handling, mode switching.

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
	// Top-down camera, like King's Bounty.
	// Fixed pitch angle, yaw rotated by mouse, zoom via scroll.

	FirstPerson    UMETA(DisplayName = "First Person"),
	// Camera inside character's head, free mouse look.
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
	// CAMERA — settings
	// ================================================================

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WoE|Camera")
	EWoE_CameraMode CurrentCameraMode;

	// Distance from character (arm length).
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WoE|Camera")
	float CurrentArmLength;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WoE|Camera")
	float MinArmLength;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WoE|Camera")
	float MaxArmLength;

	// Pitch angle for Exploration (top-down). Negative = looking down.
	// -55 ≈ King's Bounty top-down feel.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WoE|Camera",
		meta = (ClampMin = "-89", ClampMax = "0"))
	float ExplorationPitch;

	// Camera yaw in Exploration mode (rotated by mouse).
	// Not editable — driven by mouse input at runtime.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "WoE|Camera")
	float DesiredYaw;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "WoE|Camera")
	float DesiredPitch;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WoE|Camera",
		meta = (ClampMin = "0.1", ClampMax = "5.0"))
	float ExplorationYawSensitivity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WoE|Camera",
		meta = (ClampMin = "0.0", ClampMax = "150.0"))
	float FirstPersonCameraHeight;

	// Zoom speed per one scroll "click".
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WoE|Camera")
	float ZoomSpeed;

	// Interpolation speed (how fast camera transitions happen).
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WoE|Camera")
	float CameraInterpSpeed;

	// ================================================================
	// INPUT — references to Input Action assets
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
	// INPUT — handler functions
	// ================================================================

	void OnMove(const FInputActionValue& Value);
	void OnLook(const FInputActionValue& Value);
	void OnZoom(const FInputActionValue& Value);
	void OnToggleCameraMode(const FInputActionValue& Value);

	// ================================================================
	// CAMERA — internal
	// ================================================================

	void UpdateCamera(float DeltaTime);
	void ApplyCameraMode(EWoE_CameraMode NewMode);
};
