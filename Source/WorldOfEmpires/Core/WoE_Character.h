// This is the MAIN character class.
// ACharacter in UE already has:
// - Capsule (collision)
// - SkeletalMesh (3D model)
// - CharacterMovement (walk, run, jump, gravity)
// We add: camera, input, characteristics.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "WoE_Character.generated.h"

// Forward declaration.
// Tells the compiler: "this class exists, details in .cpp".
// Speeds up compilation - no need to include the full header here.
class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

// ================================================================
// ENUM for camera modes
// ================================================================

// UENUM() - macro so UE sees the enum in the editor and Blueprint.
// BlueprintType - can be used in Blueprint.
UENUM(BlueprintType)
enum class EWoE_CameraMode : uint8
	// uint8 - unsigned 8-bit integer (0-255).
	// UE requires specifying storage type for UENUM.
{
	Exploration    UMETA(DisplayName = "Exploration"),
	// Main mode: camera behind-above, like in Bounty Kid Kains.

	FirstPerson    UMETA(DisplayName = "First Person"),
	// View from character's eyes.
	// UMETA(DisplayName = "...") - how the enum appears in the editor.
};

UCLASS()
class WORLDOFEMPIRES_API AWoE_Character : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AWoE_Character();

	// Tick is called EVERY FRAME (60 times per second at 60 FPS).
	// DeltaTime = time since last frame in seconds (e.g. 0.016 for 60 FPS).
	virtual void Tick(float DeltaTime) override;

	// SetupPlayerInputComponent - UE calls this to
	// bind your input actions (WASD, mouse) to character functions.
	virtual void SetupPlayerInputComponent(
		class UInputComponent* PlayerInputComponent) override;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// ================================================================
	// COMPONENTS (parts that make up the character)
	// ================================================================

	// Spring Arm = "invisible pole" with camera at the end.
	// It automatically shortens if there's a wall between camera and character.
	// VisibleAnywhere - visible in editor, but component type cannot be changed.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "WoE|Camera")
	TObjectPtr<USpringArmComponent> CameraBoom;
	// TObjectPtr<> - UE5 "smart pointer". Like regular pointer (*),
	// but with additional checks and UE system compatibility.

	// The camera itself (renders the image to screen).
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "WoE|Camera")
	TObjectPtr<UCameraComponent> FollowCamera;

	// ================================================================
	// CAMERA - settings
	// ================================================================

	// Current camera mode.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WoE|Camera")
	EWoE_CameraMode CurrentCameraMode;
	// EditAnywhere - can be changed in editor.
	// BlueprintReadWrite - can read AND write from Blueprint.

	// Current camera distance (distance from character).
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WoE|Camera")
	float CurrentArmLength;

	// Minimum distance (at maximum zoom in).
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WoE|Camera")
	float MinArmLength;

	// Maximum distance (at maximum zoom out).
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WoE|Camera")
	float MaxArmLength;

	// Camera pitch angle (from above) for Exploration mode.
	// Negative value = looking down.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WoE|Camera")
	float ExplorationPitch;

	// Zoom speed (how fast zoom in/out per one "scroll").
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WoE|Camera")
	float ZoomSpeed;

	// Interpolation speed (smoothness of camera transition).
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WoE|Camera")
	float CameraInterpSpeed;

	// ================================================================
	// INPUT - references to Input Actions
	// ================================================================
	// Input Mapping Context and Input Actions are created as assets
	// in the editor (Content Browser). Here we store references to them.

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WoE|Input")
	TObjectPtr<UInputMappingContext> DefaultMappingContext;
	// EditDefaultsOnly - can only be changed in "template" (Blueprint / CDO),
	// not on a specific instance in the world.
	// CDO = Class Default Object - "reference" object of the class.

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WoE|Input")
	TObjectPtr<UInputAction> MoveAction;
	// "Move" action - receives 2D vector from WASD.

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WoE|Input")
	TObjectPtr<UInputAction> LookAction;
	// "Look" action - receives 2D vector from mouse.

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WoE|Input")
	TObjectPtr<UInputAction> ZoomAction;
	// "Zoom" action - receives 1D value from mouse wheel.

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WoE|Input")
	TObjectPtr<UInputAction> ToggleCameraModeAction;
	// "Toggle camera" action - button press (e.g. V).

	// ================================================================
	// INPUT functions (called by Enhanced Input system)
	// ================================================================

	// These functions are called automatically when the player presses
	// the corresponding keys. FInputActionValue contains the data
	// (e.g. direction vector for WASD).

	void OnMove(const FInputActionValue& Value);
	void OnLook(const FInputActionValue& Value);
	void OnZoom(const FInputActionValue& Value);
	void OnToggleCameraMode(const FInputActionValue& Value);

	// ================================================================
	// CAMERA - internal functions
	// ================================================================

	// Updates camera position every frame (called from Tick).
	void UpdateCamera(float DeltaTime);

	// Applies settings for a specific camera mode.
	void ApplyCameraMode(EWoE_CameraMode);

};
