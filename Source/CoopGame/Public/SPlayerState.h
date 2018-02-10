// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "SPlayerState.generated.h"

class ASBaseWeapon;

/**
 * 
 */
UCLASS()
class COOPGAME_API ASPlayerState : public APlayerState
{
	GENERATED_BODY()
	
protected:
	UFUNCTION(BlueprintCallable, Category="PlayerState")
	void AddScore(float ScoreDelta);

public:
	virtual void CopyProperties(APlayerState* PlayerState) override;
	UPROPERTY(Transient, Replicated, BlueprintReadWrite, Category = "PlayerWeapon")
		TSubclassOf<ASBaseWeapon> WeaponSelected;
private:
	
	
};
