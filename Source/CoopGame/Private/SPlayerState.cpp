// Fill out your copyright notice in the Description page of Project Settings.

#include "SPlayerState.h"
#include "Weapons/SBaseWeapon.h"
#include "Net/UnrealNetwork.h"


void ASPlayerState::AddScore(float ScoreDelta)
{
	Score += ScoreDelta;
}

void ASPlayerState::CopyProperties(APlayerState* PlayerState)
{
	Super::CopyProperties(PlayerState);
	if (PlayerState != nullptr) {
		ASPlayerState* SPlayerState = Cast<ASPlayerState>(PlayerState);
		if (SPlayerState) {
			SPlayerState->WeaponSelected = WeaponSelected;
		}
	}
}

void ASPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASPlayerState, WeaponSelected);

}