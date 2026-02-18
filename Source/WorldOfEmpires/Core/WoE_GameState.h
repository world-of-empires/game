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
    // REPLICATED PROPERTIES (свойства, которые сервер рассылает клиентам)
    // ================================================================

    // UPROPERTY() Ч макрос, который говорит UE: "отслеживай это свойство".
    // Replicated Ч свойство автоматически отправл€етс€ с сервера клиентам.
    // BlueprintReadOnly Ч можно читать из Blueprint (визуального скрипта),
    //   но нельз€ мен€ть. ћен€ть будем только из C++ на сервере.
    // Category = "WoE|World" Ч дл€ удобной группировки в редакторе.

    UPROPERTY(Replicated, BlueprintReadOnly, Category = "WoE|World")
    float GameTimeOfDay = 8.0f;
    // ¬рем€ суток в игре (0.0 = полночь, 12.0 = полдень, 24.0 = снова полночь).
    // Replicated = сервер считает врем€, клиенты получают значение автоматически.


protected:
	virtual void BeginPlay() override;

    // Ёта функци€ ќЅя«ј“≈Ћ№Ќј если есть хот€ бы одно свойство с Replicated.
    // UE вызывает еЄ автоматически, чтобы узнать  ј »≈ свойства реплицировать.
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
};
