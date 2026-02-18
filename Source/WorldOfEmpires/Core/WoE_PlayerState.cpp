// Fill out your copyright notice in the Description page of Project Settings.


#include "WoE_PlayerState.h"
#include "Net/UnrealNetwork.h" // Для репликации

AWoE_PlayerState::AWoE_PlayerState() {
	ClanId = 0; // Нет клана по умолчанию
	DisplayName = TEXT("Survivor"); // Имя по умолчанию
}

void AWoE_PlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Регистрируем наше свойство для репликации.
	// DOREPLIFETIME(ClassName, PropertyName) — макрос, который говорит:
	// "Отправляй GameTimeOfDay ВСЕМ клиентам при каждом изменении".

	DOREPLIFETIME(AWoE_PlayerState, ClanId);
	DOREPLIFETIME(AWoE_PlayerState, DisplayName);
}