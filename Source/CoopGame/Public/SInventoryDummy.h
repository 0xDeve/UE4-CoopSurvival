// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SInventoryDummy.generated.h"

class ASBaseWeapon;

UCLASS()
class COOPGAME_API ASInventoryDummy : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASInventoryDummy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Weapon")
	TSubclassOf<ASBaseWeapon> WeaponClass;
	UPROPERTY(BlueprintReadWrite, Category="Weapon")
	ASBaseWeapon* CurrentWeapon;
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
		FName WeaponAttachSocketName;
	UPROPERTY(BlueprintReadWrite, Category = "Weapon")
		bool bChanged;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	
	
};
