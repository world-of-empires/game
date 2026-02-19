// Fill out your copyright notice in the Description page of Project Settings.


#include "WoE_GameState.h"

// This include is REQUIRED for DOREPLIFETIME macro,
// which registers properties for replication.
#include "Net/UnrealNetwork.h"

AWoE_GameState::AWoE_GameState() {
	// Initial time - 8:00 AM.
	GameTimeOfDay = 8.0f;
}

void AWoE_GameState::BeginPlay() {
	Super::BeginPlay();
	UE_LOG(LogTemp, Log, TEXT("WoEGameState: BeginPlay. Time of day: %.1f"), GameTimeOfDay);
}

void AWoE_GameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Register our property for replication.
	// DOREPLIFETIME(ClassName, PropertyName) - macro that says:
	// "Send GameTimeOfDay to ALL clients on every change".
	DOREPLIFETIME(AWoE_GameState, GameTimeOfDay);
}
