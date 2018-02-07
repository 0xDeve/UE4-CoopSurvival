// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "SGameInstance.generated.h"


class ASBaseWeapon;
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
	TMap<int32, TSubclassOf<ASBaseWeapon>> SelectedWeapons;
protected:	
	
	UFUNCTION(BlueprintCallable, Category = "Weapon")
		void AddSelectedWeapons(TSubclassOf<ASBaseWeapon> SelectedWeapon, int32 PlayerID);
};
