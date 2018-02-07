// Fill out your copyright notice in the Description page of Project Settings.

#include "SInventoryDummy.h"
#include "Weapons/SBaseWeapon.h"

// Sets default values
ASInventoryDummy::ASInventoryDummy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ASInventoryDummy::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASInventoryDummy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (bChanged) {
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		CurrentWeapon = GetWorld()->SpawnActor<ASBaseWeapon>(WeaponClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
		if (CurrentWeapon) {
			CurrentWeapon->SetOwner(this);
			CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, WeaponAttachSocketName);
		}
		bChanged = false;
	}
}

// Called to bind functionality to input
void ASInventoryDummy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

