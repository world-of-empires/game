// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "WoE_GameState.generated.h"

/**
 * 
 */
UCLASS()
class WORLDOFEMPIRES_API AWoE_GameState : public AGameStateBase
{
	GENERATED_BODY()

public:
    AWoE_GameState();


    // ================================================================
    // REPLICATED PROPERTIES (properties that server sends to clients)
    // ================================================================

    // UPROPERTY() - macro that tells UE: "track this property".
    // Replicated - property is automatically sent from server to clients.
    // BlueprintReadOnly - can be read from Blueprint (visual script),
    //   but cannot be changed. We change it only from C++ on server.
    // Category = "WoE|World" - for convenient grouping in editor.

    UPROPERTY(Replicated, BlueprintReadOnly, Category = "WoE|World")
    float GameTimeOfDay = 8.0f;
    // Time of day in game (0.0 = midnight, 12.0 = noon, 24.0 = midnight again).
    // Replicated = server calculates time, clients receive value automatically.


protected:
	virtual void BeginPlay() override;

    // This function is REQUIRED if there is at least one property with Replicated.
    // UE calls it automatically to find out WHICH properties to replicate.
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
};
