// Это ГЛАВНЫЙ класс персонажа.
// ACharacter в UE уже имеет:
// — Capsule (столкновения)
// — SkeletalMesh (3D модель)
// — CharacterMovement (ходьба, бег, прыжки, гравитация)
// Мы добавляем: камеру, ввод, характеристики.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "WoE_Character.generated.h"

// Предварительное объявление (forward declaration).
// Говорим компилятору: "такой класс существует, подробности в .cpp".
// Это ускоряет компиляцию — не нужно подключать весь заголовок здесь.
class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

// ================================================================
// ENUM для режимов камеры
// ================================================================

// UENUM() — макрос, чтобы UE видел enum в редакторе и Blueprint.
// BlueprintType — можно использовать в Blueprint.
UENUM(BlueprintType)
enum class EWoE_CameraMode : uint8
	// uint8 — беззнаковое 8-битное целое (0–255).
	// UE требует указать тип хранения для UENUM.
{
	Exploration    UMETA(DisplayName = "Exploration"),
	// Основной режим: камера сзади-сверху, как в Bounty Kid Kains.

	FirstPerson    UMETA(DisplayName = "First Person"),
	// Вид из глаз персонажа.
	// UMETA(DisplayName = "...") — как enum будет отображаться в редакторе.
};

UCLASS()
class WORLDOFEMPIRES_API AWoE_Character : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AWoE_Character();

	// Tick вызывается КАЖДЫЙ КАДР (60 раз в секунду при 60 FPS).
	// DeltaTime = время с прошлого кадра в секундах (например 0.016 для 60 FPS).
	virtual void Tick(float DeltaTime) override;

	// SetupPlayerInputComponent — UE вызывает эту функцию чтобы
	// связать ваши действия ввода (WASD, мышь) с функциями персонажа.
	virtual void SetupPlayerInputComponent(
		class UInputComponent* PlayerInputComponent) override;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// ================================================================
	// КОМПОНЕНТЫ (части, из которых состоит персонаж)
	// ================================================================

	// Spring Arm = "палка-невидимка", на конце которой висит камера.
	// Она автоматически сокращается если между камерой и персонажем стена.
	// VisibleAnywhere — видна в редакторе, но нельзя менять тип компонента.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "WoE|Camera")
	TObjectPtr<USpringArmComponent> CameraBoom;
	// TObjectPtr<> — "умный указатель" UE5. Как обычный указатель (*),
	// но с доп. проверками и совместимостью с системой UE.

	// Сама камера (рендерит картинку на экран).
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "WoE|Camera")
	TObjectPtr<UCameraComponent> FollowCamera;

	// ================================================================
	// КАМЕРА — настройки
	// ================================================================

	// Текущий режим камеры.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WoE|Camera")
	EWoE_CameraMode CurrentCameraMode;
	// EditAnywhere — можно менять в редакторе.
	// BlueprintReadWrite — можно читать И менять из Blueprint.

	// Текущая дистанция камеры (расстояние от персонажа).
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WoE|Camera")
	float CurrentArmLength;

	// Минимальная дистанция (при максимальном приближении).
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WoE|Camera")
	float MinArmLength;

	// Максимальная дистанция (при максимальном отдалении).
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WoE|Camera")
	float MaxArmLength;

	// Угол наклона камеры сверху (pitch) для Exploration-режима.
	// Отрицательное значение = смотрим вниз.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WoE|Camera")
	float ExplorationPitch;

	// Скорость зума (насколько быстро приближается/отдаляется за один "щелчок").
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WoE|Camera")
	float ZoomSpeed;

	// Скорость интерполяции (плавность перехода камеры).
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WoE|Camera")
	float CameraInterpSpeed;

	// ================================================================
	// ВВОД — ссылки на Input Actions
	// ================================================================
	// Input Mapping Context и Input Actions создаются как ассеты
	// в редакторе (Content Browser). Здесь мы храним ссылки на них.

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WoE|Input")
	TObjectPtr<UInputMappingContext> DefaultMappingContext;
	// EditDefaultsOnly — можно менять только в "шаблоне" (Blueprint / CDO),
	// но не на конкретном экземпляре в мире.
	// CDO = Class Default Object — "эталонный" объект класса.

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WoE|Input")
	TObjectPtr<UInputAction> MoveAction;
	// Действие "движение" — получает 2D вектор от WASD.

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WoE|Input")
	TObjectPtr<UInputAction> LookAction;
	// Действие "обзор" — получает 2D вектор от мыши.

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WoE|Input")
	TObjectPtr<UInputAction> ZoomAction;
	// Действие "зум" — получает 1D значение от колеса мыши.

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WoE|Input")
	TObjectPtr<UInputAction> ToggleCameraModeAction;
	// Действие "переключить камеру" — нажатие кнопки (V например).

	// ================================================================
	// ФУНКЦИИ ввода (вызываются системой Enhanced Input)
	// ================================================================

	// Эти функции будут вызваны автоматически когда игрок нажимает
	// соответствующие клавиши. FInputActionValue содержит данные
	// (например, вектор направления для WASD).

	void OnMove(const FInputActionValue& Value);
	void OnLook(const FInputActionValue& Value);
	void OnZoom(const FInputActionValue& Value);
	void OnToggleCameraMode(const FInputActionValue& Value);

	// ================================================================
	// КАМЕРА — внутренние функции
	// ================================================================

	// Обновляет положение камеры каждый кадр (вызывается из Tick).
	void UpdateCamera(float DeltaTime);

	// Применяет настройки для конкретного режима камеры.
	void ApplyCameraMode(EWoE_CameraMode);

};
