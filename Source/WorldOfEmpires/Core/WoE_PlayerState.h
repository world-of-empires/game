// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "WoE_PlayerState.generated.h"

/**
 * 
 */
UCLASS()
class WORLDOFEMPIRES_API AWoE_PlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	AWoE_PlayerState();


	// ================================================================
	// Данные игрока, которые видны всем
	// ================================================================
	
	// ID клана (0 = нет клана).
	UPROPERTY(Replicated, BlueprintReadOnly, Category="WoE|Social")
	int32 ClanId;

	// Отображаемое имя в игре.
	UPROPERTY(Replicated, BlueprintReadOnly, Category="WoE|Social")
	FString DisplayName;

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
