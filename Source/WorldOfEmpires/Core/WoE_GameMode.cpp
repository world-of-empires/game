// Fill out your copyright notice in the Description page of Project Settings.


#include "WoE_GameMode.h"
#include "WoE_GameState.h"
#include "WoE_PlayerState.h"


AWoE_GameMode::AWoE_GameMode() {

    // Tell the engine: "When creating the game, use OUR classes,
    // not the default ones".

    // GameStateClass - which GameState to create for this game.
    GameStateClass = AWoE_GameState::StaticClass();
    // StaticClass() - function that returns "class description" (UClass*).
    // UE uses it to create an object of the required type.

    // PlayerStateClass - which PlayerState to create for each player.
    PlayerStateClass = AWoE_PlayerState::StaticClass();

    // DefaultPawnClass - not set yet, will be ThirdPerson by default.
    // Later we'll set AWoE_Character.
}

void AWoE_GameMode::BeginPlay() {
	Super::BeginPlay();

    // For now just log to verify everything works.
    // UE_LOG - logging macro.
    // LogTemp - log category (temporary).
    // Log - severity level (Log / Warning / Error).
    // TEXT("...") - macro for UE string literals (Unicode support).
    UE_LOG(LogTemp, Log, TEXT("WoEGameMode: BeginPlay called. Server started."))
}
