// Fill out your copyright notice in the Description page of Project Settings.

#include "SWeapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/DamageType.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "Engine/Console.h"
#include "Camera/CameraShake.h"
#include "CoopGame.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "TimerManager.h"
#include "Net/UnrealNetwork.h"

static int32 DebugWeaponLineTrace = 0;
FAutoConsoleVariableRef CVARDebugWeaponLineTrace(
	TEXT("Coop.DebugWeapon"),
	DebugWeaponLineTrace,
	TEXT("Draw debug line trace of weapon"),
	ECVF_Cheat);

// Sets default values
ASWeapon::ASWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon"));
	RootComponent = MeshComp;
	MuzzleName = "muzzle";
	BaseDamage = 20.f;
	RateOfFire = 600.f;
	RecoilY = 0.5f;
	RecoilX = 0.5f;

	SetReplicates(true);

	NetUpdateFrequency = 66.f;
	MinNetUpdateFrequency = 33.f;
}

// Called when the game starts or when spawned
void ASWeapon::BeginPlay()
{
	Super::BeginPlay();
	TimeBetweenShots = 60 / RateOfFire;
}

void ASWeapon::OnRep_HitScanTrace()
{
	// Play cosmetic effect
	FireEffect(HitScanTrace.TraceTo);
	PlayImpactEffect(HitScanTrace.SurfaceType, HitScanTrace.TraceTo);
}

void ASWeapon::ServerFire_Implementation()
{
	Fire();
}

bool ASWeapon::ServerFire_Validate()
{
	return true;
}

void ASWeapon::Fire()
{
	if (Role < ROLE_Authority) {
		ServerFire();
		
	}
	AActor* MyPawn = GetOwner();
	if (MyPawn) {
		FVector EyeLocation;
		FRotator EyeRotation;
		MyPawn->GetActorEyesViewPoint(EyeLocation, EyeRotation);
		APlayerController* PC = Cast<APlayerController>(MyPawn->GetInstigatorController());
		if (PC)
			PC->ClientPlayCameraShake(CamShake);

		FVector ShotDirection = EyeRotation.Vector();
		FVector TraceEnd = EyeLocation + (ShotDirection * 10000);

		/** Apply recoil */
		PC->AddPitchInput(RecoilY);
		PC->AddYawInput(RecoilX);

		FCollisionQueryParams QueryParam;
		QueryParam.AddIgnoredActor(MyPawn);
		QueryParam.AddIgnoredActor(this);
		QueryParam.bTraceComplex = true; // Trace into object triangles instead of collision box
		QueryParam.bReturnPhysicalMaterial = true;
		FVector TracerEndPoint = TraceEnd;
		FHitResult Hit;

		EPhysicalSurface SurfaceType = SurfaceType_Default;

		if (GetWorld()->LineTraceSingleByChannel(Hit, EyeLocation, TraceEnd, ECollisionChannel::COLLISION_WEAPON, QueryParam)) {
			
			AActor* HitActor = Hit.GetActor();
			if (DebugWeaponLineTrace > 0)
				DrawDebugLine(GetWorld(), EyeLocation, TraceEnd, FColor::Red, false, 1.0f, 0, 1.0f);

			SurfaceType = UPhysicalMaterial::DetermineSurfaceType(Hit.PhysMaterial.Get());

			float ActualDamage = BaseDamage;

			if (SurfaceType == SURFACE_FLESHVULNERABLE)
				ActualDamage *= 5.0f;

			UGameplayStatics::ApplyPointDamage(HitActor, ActualDamage, ShotDirection, Hit, MyPawn->GetInstigatorController(), this, DamageType);

			PlayImpactEffect(SurfaceType, Hit.ImpactPoint);

			TracerEndPoint = Hit.ImpactPoint;

		}
		FireEffect(TracerEndPoint);
		if (Role == ROLE_Authority) {
			HitScanTrace.TraceTo = TracerEndPoint;
			HitScanTrace.SurfaceType = SurfaceType;
		}
		LastFireTime = GetWorld()->TimeSeconds;
		UGameplayStatics::PlaySoundAtLocation(this, ShootingEffect, GetActorLocation());
	}
}

void ASWeapon::StartFire()
{
	float FirstDelay = FMath::Max(LastFireTime + TimeBetweenShots - GetWorld()->TimeSeconds, 0.0f);
	GetWorldTimerManager().SetTimer(TimerHandle, this, &ASWeapon::Fire, TimeBetweenShots, true, FirstDelay);
}

void ASWeapon::StopFire()
{
	GetWorldTimerManager().ClearTimer(TimerHandle);
}

void ASWeapon::FireEffect(FVector TracerEndPoint)
{
	if (MuzzleSplash != nullptr) {
		UGameplayStatics::SpawnEmitterAttached(MuzzleSplash, MeshComp, MuzzleName);
	}

	if (SmokeTrail != nullptr) {
		FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleName);
		UParticleSystemComponent* SmokeTrailComp = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SmokeTrail, MuzzleLocation);
		if (SmokeTrailComp) {
			SmokeTrailComp->SetVectorParameter("BeamEnd", TracerEndPoint);
		}
	}
}

void ASWeapon::PlayImpactEffect(EPhysicalSurface SurfaceType, FVector ImpactPoint)
{
	UParticleSystem* SelectedEffect = nullptr;
	switch (SurfaceType) {
	case SURFACE_FLESHDEFAULT:
	case SURFACE_FLESHVULNERABLE:
		SelectedEffect = FleshImpactEffect;
		break;
	default:
		SelectedEffect = ImpactEffect;
	}

	if (SelectedEffect) {
		FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleName);
		FVector ShotDirection = ImpactPoint - MuzzleLocation;
		ShotDirection.Normalize();
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SelectedEffect, ImpactPoint, ShotDirection.Rotation());
	}
		
}

// Called every frame
void ASWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ASWeapon, HitScanTrace, COND_SkipOwner);
}