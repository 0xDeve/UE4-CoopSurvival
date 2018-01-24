// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "STrackerBot.generated.h"

class UStaticMeshComponent;
class USHealthComponent;
class USphereComponent;
class USoundCue;
UCLASS()
class COOPGAME_API ASTrackerBot : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ASTrackerBot();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UPROPERTY(VisibleDefaultsOnly, Category="Components")
	UStaticMeshComponent* MeshComp;
	UFUNCTION()
	void HandleTakeDamage(USHealthComponent* OwningHealthComp, float Health, float HealthData, const class UDamageType* DamageType,
		class AController* InstigatedBy, AActor* DamageCauser);
	// Dynamic material to pulse on damage
	UMaterialInstanceDynamic* MatInst;

	FVector GetNextPathPoint();
	// Next point in navigation path
	FVector NextPathPoint;
	UPROPERTY(EditDefaultsOnly, Category = "Trackerbot")
	float MovementForce;
	UPROPERTY(EditDefaultsOnly, Category = "Trackerbot")
	bool bUseVelocityChange;
	UPROPERTY(EditDefaultsOnly, Category = "Trackerbot")
	float RequiredDistanceToTarget;
	
	USHealthComponent* HealthComp;
	UPROPERTY(EditDefaultsOnly, Category = "Trackerbot")
	UParticleSystem* ExplosionEffect;
	void SelfDestruct();
	bool bExploded;
	UPROPERTY(EditDefaultsOnly, Category = "Trackerbot")
	float ExplosionDamage;
	UPROPERTY(EditDefaultsOnly, Category = "Trackerbot")
	float ExplosionRadius;
	UPROPERTY(VisibleAnywhere, Category = "Trackerbot")
	USphereComponent* SphereComp;

	UPROPERTY(EditDefaultsOnly, Category = "Trackerbot")
	USoundCue* SelfDestructSound;
	UPROPERTY(EditDefaultsOnly, Category = "Trackerbot")
	USoundCue* ExplodeSound;
	FTimerHandle TimerHandle_SelfDamage;

	void DamageSelf();
	float SelfDamageInterval;
	bool bStartedSelfDestruction;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
};
