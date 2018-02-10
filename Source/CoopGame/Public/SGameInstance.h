// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "SGameInstance.generated.h"


class ASBaseWeapon;
class ASPlayerController;
/**
 * 
 */
UCLASS()
class COOPGAME_API USGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	/*UPROPERTY(VisibleDefaultsOnly,BlueprintReadWrite, Category="Weapon")
	TArray<TSubclassOf<ASBaseWeapon>> SelectedWeapons;*/
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category="Weapon")
	TMap<int32, TSubclassOf<ASBaseWeapon>> SelectedWeapons;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
		TArray<ASPlayerController*> ConnectedPlayers;
protected:	
	
	UFUNCTION(BlueprintCallable, Category = "Weapon")
		void AddSelectedWeapons(TSubclassOf<ASBaseWeapon> SelectedWeapon, int32 PlayerID);
};
