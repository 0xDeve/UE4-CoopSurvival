// Fill out your copyright notice in the Description page of Project Settings.

#include "SPowerUpActor.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ASPowerUpActor::ASPowerUpActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PowerupInterval = 0.0f;
	TotalNrOfTicks = 0;
	bIsPowerUpActive = false;
	SetReplicates(true);
}


void ASPowerUpActor::OnTickPowerup()
{
	TicksProcessed++;

	OnPowerupTick();

	if (TicksProcessed >= TotalNrOfTicks) {
		OnExpire();
		bIsPowerUpActive = false;
		OnRep_PowerupActive();
		// Delete timer
		GetWorldTimerManager().ClearTimer(TimerHandle_PowerupTick);
	}
}

void ASPowerUpActor::OnRep_PowerupActive()
{
	OnPowerupStateChanged(bIsPowerUpActive);
}

void ASPowerUpActor::ActivatePowerup(AActor* ActivateFor)
{
	OnActivated(ActivateFor);
	bIsPowerUpActive = true;
	OnRep_PowerupActive();
	if (PowerupInterval > 0.0f) {

		GetWorldTimerManager().SetTimer(TimerHandle_PowerupTick, this, &ASPowerUpActor::OnTickPowerup, PowerupInterval, true);
	}
	else
		OnTickPowerup();
}

void ASPowerUpActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASPowerUpActor, bIsPowerUpActive);
}