// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ExplosiveBarrel.generated.h"


class USHealthComponent;
class UParticleSystem;
class UStaticMeshComponent;
class URadialForceComponent;
class USoundCue;
UCLASS()
class COOPGAME_API AExplosiveBarrel : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AExplosiveBarrel();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(ReplicatedUsing=OnRep_Exploded)
	bool bExploded;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USHealthComponent* HealthComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effect")
	UParticleSystem* Explosion;

	UPROPERTY(VisibleAnywhere, Category = "Effect")
	UStaticMeshComponent* BarrelComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	URadialForceComponent* ExplosionComp;
	UFUNCTION()
	void OnHealthChanged(USHealthComponent* OwningHealthComp, float Health, float HealthData, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);
	UFUNCTION()
		void OnRep_Exploded();
	UPROPERTY(EditDefaultsOnly, Category = "FX")
		UMaterialInterface* ExplodedMaterial;

	UPROPERTY(EditDefaultsOnly, Category = "FX")
	USoundCue* ExplosionSound;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
	
};
