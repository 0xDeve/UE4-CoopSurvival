// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SPowerUpActor.generated.h"

UCLASS()
class COOPGAME_API ASPowerUpActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASPowerUpActor();

protected:


	/* Time between powerup tick */
	UPROPERTY(EditDefaultsOnly, Category="Powerups")
		float PowerupInterval;

	/* Total times we apply the powerup effect */ 
	UPROPERTY(EditDefaultsOnly, Category = "Powerups")
		int32 TotalNrOfTicks;

	FTimerHandle TimerHandle_PowerupTick;

	int32 TicksProcessed;
	UFUNCTION()
		void OnTickPowerup();
	// Keeps state of powerup
	UPROPERTY(ReplicatedUsing=OnRep_PowerupActive)
	bool bIsPowerUpActive;
	UFUNCTION()
		void OnRep_PowerupActive();
	UFUNCTION(BlueprintImplementableEvent, Category = "Powerups")
	void OnPowerupStateChanged(bool bNewIsActive);
public:	

	void ActivatePowerup(AActor* ActivateFor);

	UFUNCTION(BlueprintImplementableEvent, Category="Powerups")
		void OnActivated(AActor* ActivateFor);

	UFUNCTION(BlueprintImplementableEvent, Category = "Powerups")
		void OnExpire();

	UFUNCTION(BlueprintImplementableEvent, Category = "Powerups")
		void OnPowerupTick();
};
