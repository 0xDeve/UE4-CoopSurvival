// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SWeapon.h"
#include "GrenadeLauncher.generated.h"

class AGrenadeProjectile;
/**
 * 
 */
UCLASS()
class COOPGAME_API AGrenadeLauncher : public ASWeapon
{
	GENERATED_BODY()
protected:
	//UFUNCTION(BlueprintCallable, Category = "Weapon")
	virtual void Fire() override;


public:
	/** Projectile class to spawn */
	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	TSubclassOf<AGrenadeProjectile> ProjectileClass;

};
