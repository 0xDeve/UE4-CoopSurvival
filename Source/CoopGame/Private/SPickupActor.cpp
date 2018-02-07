// Fill out your copyright notice in the Description page of Project Settings.

#include "SPickupActor.h"
#include "Components/SphereComponent.h"
#include "Components/DecalComponent.h"
#include "SPowerUpActor.h"
#include "SCharacter.h"
// Sets default values
ASPickupActor::ASPickupActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere comp"));
	SphereComp->SetSphereRadius(75.0f);
	RootComponent = SphereComp;

	DecalComp = CreateDefaultSubobject<UDecalComponent>(TEXT("Decal comp"));
	DecalComp->SetupAttachment(RootComponent);
	DecalComp->DecalSize = FVector(64, 75, 75);
	DecalComp->SetRelativeRotation(FRotator(90.f, 0, 0));

	CoolDownDuration = 10.0f;
	SetReplicates(true);
}

// Called when the game starts or when spawned
void ASPickupActor::BeginPlay()
{
	Super::BeginPlay();
	if (Role == ROLE_Authority) {
		Respawn();
	}
}

void ASPickupActor::Respawn()
{
	if (PowerupClass == nullptr) {
		UE_LOG(LogTemp, Warning, TEXT("Power up class blueprint not found"));
		return;
	}
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	PowerupInstance = GetWorld()->SpawnActor<ASPowerUpActor>(PowerupClass, GetTransform(), SpawnParams);
}

void ASPickupActor::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);
	if (Role == ROLE_Authority) {
		if (PowerupInstance) {
			ASCharacter* OverlappedChar = Cast<ASCharacter>(OtherActor);
			if (OverlappedChar) {
				PowerupInstance->ActivatePowerup(OtherActor);
				PowerupInstance = nullptr;

				GetWorldTimerManager().SetTimer(TimerHandle_Respawn, this, &ASPickupActor::Respawn, CoolDownDuration);
			}
		}
	}
}
