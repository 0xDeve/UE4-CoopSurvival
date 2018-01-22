// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SBaseWeapon.generated.h"

class USkeletalMeshComponent;
class UDamageType;
class UParticleSystem;
class USlateBrushAsset;
// Contains information of a single line-trace weapon linetrace
USTRUCT()
struct FHitScanTracer {
	GENERATED_BODY()
public:
	UPROPERTY()
		TEnumAsByte<EPhysicalSurface> SurfaceType;
	UPROPERTY()
		FVector_NetQuantize TraceTo;
};

UCLASS()
class COOPGAME_API ASBaseWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASBaseWeapon();
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	// Whether the gun can zoom or not
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
		bool Zoomable;
	// Zoom FOV, only available for edit if the weapon can zoom
	UPROPERTY(EditDefaultsOnly, meta = (EditCondition = Zoomable), Category = "Weapon")
		float ZoomFOV;
	// Zoom interp speed, value between 0-100, and only available for edit if weapon can zoom
	UPROPERTY(EditDefaultsOnly, meta = (ClampMin = 0.0f, ClampMax = 100.f, EditCondition = Zoomable), Category = "Weapon")
		float ZoomInterpSpeed;
	// Fire function
	void Fire();
	// Set timer for fire function
	void StartFire();
	// Clear timer for fire function
	void StopFire();
	void Reload();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Timer
	FTimerHandle TimerHandle;
	// Base mesh component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		USkeletalMeshComponent* MeshComp;
	// Type of damage
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
		TSubclassOf<UDamageType> DamageType;
	// Muzzlesplash FX
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
		UParticleSystem* MuzzleSplash;
	// Impact FX
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
		UParticleSystem* ImpactEffect;
	// Player hit impact FX
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
		UParticleSystem* FleshImpactEffect;
	// Smoke trail
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
		UParticleSystem* SmokeTrail;
	// Muzzlesplash socket name
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
		FName MuzzleName;
	// Recoil Y
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
		float RecoilY;
	// Recoil X
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
		float RecoilX;
	// Camera shake 
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
		TSubclassOf<UCameraShake> CamShake;
	// Base gun damage
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
		float BaseDamage;
	// Rate of fire ( how many bullets per second )
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
		float RateOfFire;
	// Firing sound effect
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
		USoundBase* FireSound;
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
		float Weight;
	// Weapon attributes
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ammunition")
		int32 TotalAmmo;
	UPROPERTY(EditDefaultsOnly, Category = "Ammunition")
		int32 AmmoPerMag;
	UPROPERTY(EditDefaultsOnly, Category = "Ammunition")
		float ReloadTime;
	UPROPERTY(BlueprintReadOnly, Category = "Ammunition")
		int32 CurrentAmmo;
	UPROPERTY(BlueprintReadOnly, Category = "Ammunition")
		bool bOutOfAmmo;
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
		float BulletMaxDistance;
	// Weapon image display
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
		USlateBrushAsset* WeaponDisplay;
	// Fire server function
	UFUNCTION(Server, Reliable, WithValidation)
		void ServerFire();
	UFUNCTION(Server, Reliable, WithValidation)
		void ServerReload();

	UPROPERTY(ReplicatedUsing = OnRep_HitScanTrace)
		FHitScanTracer HitScanTrace;
	UFUNCTION()
		void OnRep_HitScanTrace();
	// Delay between each shot ( = 60 / RateOfFire )
	float TimeBetweenShots;
	void PlayImpactEffect(EPhysicalSurface SurfaceType, FVector ImpactPoint);
	void FireEffect(FVector TracerEndPoint);
private:

	float LastFireTime;
	
	
};
