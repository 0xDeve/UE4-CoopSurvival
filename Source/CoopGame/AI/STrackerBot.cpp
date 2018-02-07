// Fill out your copyright notice in the Description page of Project Settings.

#include "STrackerBot.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "AI/Navigation/NavigationSystem.h"
#include "AI/Navigation/NavigationPath.h"
#include "GameFramework/Character.h"
#include "Components/SHealthComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SphereComponent.h"
#include "SCharacter.h"
#include "Sound/SoundCue.h"
// Sets default values
ASTrackerBot::ASTrackerBot()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetSimulatePhysics(true);
	RootComponent = MeshComp;
	MeshComp->SetCanEverAffectNavigation(false);
	HealthComp = CreateDefaultSubobject<USHealthComponent>(TEXT("HealthComp"));
	HealthComp->OnHealthChanged.AddDynamic(this, &ASTrackerBot::HandleTakeDamage);
	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere comp"));
	SphereComp->SetSphereRadius(200);
	SphereComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	SphereComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	SphereComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	SphereComp->SetupAttachment(RootComponent);
	bUseVelocityChange = true;
	MovementForce = 1000;
	RequiredDistanceToTarget = 100;
	ExplosionDamage = 40;
	ExplosionRadius = 200;
	SelfDamageInterval = 0.25f;
}

// Called when the game starts or when spawned
void ASTrackerBot::BeginPlay()
{
	Super::BeginPlay();
	// Find initial path point
	if (Role == ROLE_Authority) {
		NextPathPoint = GetNextPathPoint();
		FTimerHandle TimerHandle_DetectBot;
		GetWorldTimerManager().SetTimer(TimerHandle_DetectBot, this, &ASTrackerBot::DetectBot, 1.0f, true);
	}
}

void ASTrackerBot::HandleTakeDamage(USHealthComponent* OwningHealthComp, float Health, float HealthData, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	// Explode on hitpoints == 0
	UE_LOG(LogTemp, Warning, TEXT("%f"),Health);
	if (MatInst == nullptr)
		MatInst = MeshComp->CreateAndSetMaterialInstanceDynamicFromMaterial(0, MeshComp->GetMaterial(0));
	if (MatInst)
		MatInst->SetScalarParameterValue("LastTimeDamageTaken", GetWorld()->TimeSeconds);
	if (Health <= 0)
		SelfDestruct();
}

FVector ASTrackerBot::GetNextPathPoint()
{
	auto PlayerPawn = UGameplayStatics::GetPlayerCharacter(this, 0);
	UNavigationPath* NavPath = UNavigationSystem::FindPathToActorSynchronously(this, GetActorLocation(), PlayerPawn);
	if (NavPath) {
		if (NavPath->PathPoints.Num() > 1)
			return NavPath->PathPoints[1];
	}
	return GetActorLocation();
}

void ASTrackerBot::SelfDestruct()
{
	if (bExploded) {
		return;
	}
	bExploded = true;
	UE_LOG(LogTemp, Warning, TEXT("Exploded"));
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionEffect, GetActorLocation());
	UGameplayStatics::PlaySoundAtLocation(this, ExplodeSound, GetActorLocation());
	MeshComp->SetVisibility(false, true);
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	if (Role == ROLE_Authority) {
		TArray<AActor*> IgnoredActors;
		IgnoredActors.Add(this);
		float ActualDamage = ExplosionDamage + (ExplosionDamage * PowerLevel);
		UGameplayStatics::ApplyRadialDamage(this, ActualDamage, GetActorLocation(), ExplosionRadius, nullptr, IgnoredActors, this, GetInstigatorController(), true);
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionEffect, GetActorLocation());
		// Delete actor
		SetLifeSpan(2.0f);
	}
}

void ASTrackerBot::DamageSelf()
{
	UGameplayStatics::ApplyDamage(this, 20, GetInstigatorController(), this, nullptr);
}

// Called every frame
void ASTrackerBot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (Role == ROLE_Authority && !bExploded) {
		float DistanceToTarget = (GetActorLocation() - NextPathPoint).Size();
		if (DistanceToTarget <= RequiredDistanceToTarget) {
			NextPathPoint = GetNextPathPoint();
		}
		else {
			// Keep moving towards next target
			FVector ForceDirection = NextPathPoint - GetActorLocation();
			ForceDirection.Normalize();

			ForceDirection *= MovementForce;
			MeshComp->AddForce(ForceDirection, NAME_None, bUseVelocityChange);
		}
	}
}

void ASTrackerBot::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);
	if (!bStartedSelfDestruction && !bExploded) {
		ASCharacter* PlayerPawn = Cast<ASCharacter>(OtherActor);
		if (PlayerPawn) {
			if (Role == ROLE_Authority) {
				// Start self destruction sequence
				GetWorldTimerManager().SetTimer(TimerHandle_SelfDamage, this, &ASTrackerBot::DamageSelf, SelfDamageInterval, true, 0.0f);
				UE_LOG(LogTemp, Warning, TEXT("Exploded"));
			}
			UGameplayStatics::SpawnSoundAttached(SelfDestructSound, RootComponent);
			bStartedSelfDestruction = true;
		}
	}

}

void ASTrackerBot::DetectBot()
{
	const float Radius = 600.f;
	FCollisionShape TempCollisionComp;
	TempCollisionComp.SetSphere(Radius);
	
	FCollisionObjectQueryParams QueryParams;
	QueryParams.AddObjectTypesToQuery(ECC_PhysicsBody);
	QueryParams.AddObjectTypesToQuery(ECC_Pawn);

	TArray<FOverlapResult> Overlaps;
	GetWorld()->OverlapMultiByObjectType(Overlaps, GetActorLocation(), FQuat::Identity, QueryParams, TempCollisionComp);


	int32 NrOfBots = 0;
	for (FOverlapResult Result : Overlaps) {
		ASTrackerBot* Bot = Cast<ASTrackerBot>(Result.GetActor());
		if (Bot && Bot != this)
			NrOfBots++;
	}

	const int32 MaxPowerLevel = 4;
	
	PowerLevel = FMath::Clamp(NrOfBots, 0, MaxPowerLevel);
	if (MatInst == nullptr) {
		MatInst = MeshComp->CreateAndSetMaterialInstanceDynamicFromMaterial(0, MeshComp->GetMaterial(0));
	}
	if (MatInst) {
		// Convert to a float between 0 and 1 just like an 'Alpha' value of a texture. Now the material can be set up without having to know the max power level 
		// which can be tweaked many times by gameplay decisions (would mean we need to keep 2 places up to date)
		float Alpha = PowerLevel / (float)MaxPowerLevel;
		MatInst->SetScalarParameterValue("PowerLevelAlpha", Alpha);
	}
}

