// Fill out your copyright notice in the Description page of Project Settings.

#include "ExplosiveBarrel.h"
#include "Components/SHealthComponent.h"
#include "Particles/ParticleSystem.h"
#include "Kismet/GameplayStatics.h"
#include "Components/StaticMeshComponent.h"
#include "PhysicsEngine/RadialForceComponent.h"
#include "Net/UnrealNetwork.h"
#include "Sound/SoundCue.h"
// Sets default values
AExplosiveBarrel::AExplosiveBarrel()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	HealthComp = CreateDefaultSubobject<USHealthComponent>(TEXT("Health Component"));
	
	BarrelComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Barrel"));
	RootComponent = BarrelComp;
	ExplosionComp = CreateDefaultSubobject<URadialForceComponent>(TEXT("Explosion component"));
	ExplosionComp->SetupAttachment(BarrelComp);
	ExplosionComp->Radius = 250;
	ExplosionComp->bImpulseVelChange = true;
	ExplosionComp->bAutoActivate = false; // Prevent component from ticking, and only use FireImpulse() instead
	ExplosionComp->bIgnoreOwningActor = true; // ignore self
	HealthComp->OnHealthChanged.AddDynamic(this, &AExplosiveBarrel::OnHealthChanged);
	SetReplicates(true);
	SetReplicateMovement(true);
	NetUpdateFrequency = 66.f;
	MinNetUpdateFrequency = 33.f;
	BarrelComp->SetCanEverAffectNavigation(false);
}

// Called when the game starts or when spawned
void AExplosiveBarrel::BeginPlay()
{
	Super::BeginPlay();
	
}

void AExplosiveBarrel::OnHealthChanged(USHealthComponent* OwningHealthComp, float Health, float HealthData, const class UDamageType* DamageType,
	class AController* InstigatedBy, AActor* DamageCauser)
{


	if (Health <= 0.0f && bExploded == false) {
		if (Explosion) {
			OnRep_Exploded();
		}
		ExplosionComp->FireImpulse();
		BarrelComp->AddImpulse(FVector::UpVector * 400.f, NAME_None, true);
		bExploded = true;
	}
}

void AExplosiveBarrel::OnRep_Exploded()
{
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Explosion, GetActorLocation(), GetActorRotation());
	BarrelComp->SetMaterial(0,ExplodedMaterial);
	if (ExplosionSound)
		UGameplayStatics::PlaySoundAtLocation(this, ExplosionSound, GetActorLocation());
}

// Called every frame
void AExplosiveBarrel::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AExplosiveBarrel::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AExplosiveBarrel, bExploded);
}