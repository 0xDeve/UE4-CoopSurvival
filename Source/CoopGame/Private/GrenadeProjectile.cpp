// Fill out your copyright notice in the Description page of Project Settings.

#include "GrenadeProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "DrawDebugHelpers.h"

// Sets default values
AGrenadeProjectile::AGrenadeProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->SetCollisionProfileName("Projectile");
	CollisionComp->OnComponentHit.AddDynamic(this, &AGrenadeProjectile::OnHit);	// set up a notification for when this component hits something blocking

																			// Players can't walk on it
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;

	// Set as root component
	RootComponent = CollisionComp;

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 3000.f;
	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;

	// Die after 3 seconds by default
	InitialLifeSpan = 3.0f;

	ExplosionDelay = 1.f;
}

// Called when the game starts or when spawned
void AGrenadeProjectile::BeginPlay()
{
	Super::BeginPlay();
	FTimerHandle ExplodeTimer;
	GetWorldTimerManager().SetTimer(ExplodeTimer, this, &AGrenadeProjectile::Explode, ExplosionDelay, false);
}

void AGrenadeProjectile::Explode()
{
	if (ImpactEffect)
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactEffect, GetActorLocation(), GetActorRotation());
	UGameplayStatics::ApplyRadialDamage(GetWorld(), 75.f, CollisionComp->RelativeLocation, 200.f, DamageType, TArray<AActor*>(), this, nullptr, true);
	Destroy();
}

// Called every frame
void AGrenadeProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AGrenadeProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (ImpactEffect)
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactEffect, Hit.ImpactPoint, Hit.ImpactNormal.Rotation());
	UGameplayStatics::ApplyRadialDamage(GetWorld(), 75.f, CollisionComp->RelativeLocation, 200.f, DamageType, TArray<AActor*>(),this,nullptr,true);
	DrawDebugSphere(GetWorld(), CollisionComp->RelativeLocation, 200.f, 32, FColor::Red, true, 10.0f, 0, 1.f);
	Destroy();
}