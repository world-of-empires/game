// Fill out your copyright notice in the Description page of Project Settings.


#include "WoE_GameState.h"

// Ётот include ќЅя«ј“≈Ћ≈Ќ дл€ макроса DOREPLIFETIME,
// который регистрирует свойства дл€ репликации.
#include "Net/UnrealNetwork.h"

AWoE_GameState::AWoE_GameState() {
	// Ќачальное врем€ - 8:00 утра.
	GameTimeOfDay = 8.0f;
}

void AWoE_GameState::BeginPlay() {
	Super::BeginPlay();
	UE_LOG(LogTemp, Log, TEXT("WoEGameState: BeginPlay. ¬рем€ суток: %.1f"), GameTimeOfDay);
}

void AWoE_GameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// –егистрируем наше свойство дл€ репликации.
	// DOREPLIFETIME(ClassName, PropertyName) Ч макрос, который говорит:
	// "ќтправл€й GameTimeOfDay ¬—≈ћ клиентам при каждом изменении".
	DOREPLIFETIME(AWoE_GameState, GameTimeOfDay);
}