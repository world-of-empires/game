// Fill out your copyright notice in the Description page of Project Settings.


#include "WoE_PlayerState.h"
#include "Net/UnrealNetwork.h" // For replication

AWoE_PlayerState::AWoE_PlayerState() {
	ClanId = 0; // No clan by default
	DisplayName = TEXT("Survivor"); // Default name
}

void AWoE_PlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Register our properties for replication.
	// DOREPLIFETIME(ClassName, PropertyName) - macro that says:
	// "Send to ALL clients on every change".

	DOREPLIFETIME(AWoE_PlayerState, ClanId);
	DOREPLIFETIME(AWoE_PlayerState, DisplayName);
}
