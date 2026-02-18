// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "WoE_GameMode.generated.h"

/**
 * 
 */
UCLASS()
class WORLDOFEMPIRES_API AWoE_GameMode : public AGameModeBase
{
	GENERATED_BODY()
	

public:
	AWoE_GameMode();

protected:
	virtual void BeginPlay() override;
};
