// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
class ASBaseWeapon;
class UCameraShake;
class USHealthComponent;

UCLASS()
class COOPGAME_API ASCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	/** Move forward and backward with W-D*/
	void MoveForward(float Value);
	/** Move left and right with A-S*/
	void MoveRight(float Value);
	/** Third person camera*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	UCameraComponent* ThirdPersonCamera;
	/** Spring arm for camera attachment*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USpringArmComponent* SpringArmComp;
	/** Crouch action */
	void BeginCrouch();
	/** Player zoom status*/
	UPROPERTY(BlueprintReadOnly, Category="Weaponw")
	bool bWantsToZoom;
	/** Zoom FOV, can be set through blueprints*/
	UPROPERTY(EditDefaultsOnly,Category = "Weapon")
	float ZoomFOV;
	/** Zoom speed*/
	UPROPERTY(EditDefaultsOnly, Category = "Weapon", meta=(ClampMin = 0.0f, ClampMax = 100.f))
	float ZoomInterpSpeed;
	/** Default value of 90*/
	float DefaultFOV;

	void BeginZoom();
	void EndZoom();

	UPROPERTY(EditDefaultsOnly, Category="Weapon")
	TSubclassOf<ASBaseWeapon> MainWeaponClass;
	
	UPROPERTY(Replicated, BlueprintReadOnly)
	ASBaseWeapon* CurrentWeapon;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	FName WeaponAttachSocketName;
	
	void StartFire();

	UPROPERTY(BlueprintReadOnly, Category = "Character")
	bool bIsShooting;

	UPROPERTY(BlueprintReadOnly, Category = "Character")
	bool bIsSprinting;

	UPROPERTY(BlueprintReadOnly, Category = "Character")
	bool bIsProning;

	UPROPERTY(BlueprintReadOnly, Category = "Character")
		bool bIsCrouching;
	void StopFire();

	void StartSprint();
	void StopSprint();
	void StartProne();
	void StopProne();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	USHealthComponent* HealthComp;

	UFUNCTION()
	void OnHealthChanged(USHealthComponent* OwningHealthComp, float Health, float HealthData, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);
	
	UPROPERTY(Replicated,BlueprintReadOnly, Category="Editor")
	bool bDied;

	void StartReload();
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual FVector GetPawnViewLocation() const override;
	
};
