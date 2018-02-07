// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SPickupActor.generated.h"

class USphereComponent;
class UDecalComponent;
class ASPowerUpActor;
class ASCharacter;
UCLASS()
class COOPGAME_API ASPickupActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASPickupActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UPROPERTY(VisibleAnywhere, Category="Components")
	USphereComponent* SphereComp;
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UDecalComponent* DecalComp;
	UPROPERTY(EditInstanceOnly,Category="Pickup actor")
	TSubclassOf<ASPowerUpActor> PowerupClass;

	void Respawn();

	ASPowerUpActor* PowerupInstance;
	UPROPERTY(EditInstanceOnly, Category = "Pickup actor")
	float CoolDownDuration;

	FTimerHandle TimerHandle_Respawn;
public:	
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

	
	
};
