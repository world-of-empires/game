// Fill out your copyright notice in the Description page of Project Settings.


#include "WoE_GameMode.h"
#include "WoE_GameState.h"
#include "WoE_PlayerState.h"


AWoE_GameMode::AWoE_GameMode() {

    // √оворим движку : " огда создаЄшь игру, используй ЌјЎ» классы,
    // а не стандартные".

    // GameStateClass Ч какой GameState создать дл€ этой игры.
    GameStateClass = AWoE_GameState::StaticClass();
    // StaticClass() Ч функци€, котора€ возвращает "описание класса" (UClass*).
    // UE использует его чтобы создать объект нужного типа.

    // PlayerStateClass Ч какой PlayerState создавать дл€ каждого игрока.
    PlayerStateClass = AWoE_PlayerState::StaticClass();

    // DefaultPawnClass пока не трогаем Ч будет ThirdPerson по умолчанию.
    // ѕозже поставим AWoECharacter.

}

void AWoE_GameMode::BeginPlay() {
	Super::BeginPlay();

    // ѕока просто пишем в лог, чтобы убедитьс€ что всЄ работает.
    // UE_LOG Ч макрос логировани€.
    // LogTemp Ч категори€ лога (временна€).
    // Log Ч уровень серьЄзности (Log / Warning / Error).
    // TEXT("...") Ч макрос дл€ строковых литералов UE (поддержка Unicode).
    UE_LOG(LogTemp, Log, TEXT("WoEGameMode: BeginPlay вызван. —ервер запущен."))
}