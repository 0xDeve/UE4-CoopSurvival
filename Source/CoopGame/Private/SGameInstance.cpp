// Fill out your copyright notice in the Description page of Project Settings.

#include "SGameInstance.h"
#include "Weapons/SBaseWeapon.h"
#include "SPlayerController.h"


void USGameInstance::AddSelectedWeapons(TSubclassOf<ASBaseWeapon> SelectedWeapon, int32 PlayerID)
{
	SelectedWeapons.Add(PlayerID, SelectedWeapon);
}
