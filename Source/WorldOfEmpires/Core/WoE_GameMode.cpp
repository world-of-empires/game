// Fill out your copyright notice in the Description page of Project Settings.


#include "WoE_GameMode.h"
#include "WoE_GameState.h"
#include "WoE_PlayerState.h"


AWoE_GameMode::AWoE_GameMode() {
    GameStateClass = AWoE_GameState::StaticClass();  // GameStateClass — какой GameState создать для этой игры
    PlayerStateClass = AWoE_PlayerState::StaticClass(); // PlayerStateClass — какой PlayerState создавать для каждого игрока.

    // Назначаем Blueprint-персонажа как класс по умолчанию.
    // ConstructorHelpers::FClassFinder ищет Blueprint-класс по пути.
    // Путь берётся из Content Browser (правая кнопка ? Copy Reference).
    static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/WoE/Core/Characters/BP_WoECharacter"));
    // /Game/ = папка Content/
    // Дальше путь как в Content Browser, но без расширения файла.

    if (PlayerPawnBPClass.Class != nullptr)
    {
        DefaultPawnClass = PlayerPawnBPClass.Class;
    }
}

void AWoE_GameMode::BeginPlay() {
	Super::BeginPlay();
    UE_LOG(LogTemp, Log, TEXT("WoEGameMode: BeginPlay called. Server started."))
}
