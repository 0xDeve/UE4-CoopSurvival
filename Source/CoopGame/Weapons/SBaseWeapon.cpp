// Fill out your copyright notice in the Description page of Project Settings.

#include "SBaseWeapon.h"
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
#include "Slate/SlateBrushAsset.h"

// Sets default values
ASBaseWeapon::ASBaseWeapon()
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
	BulletMaxDistance = 10000.f;
	SetReplicates(true);
	NetUpdateFrequency = 66.f;
	MinNetUpdateFrequency = 33.f;
	bOutOfAmmo = false;

}

// Called when the game starts or when spawned
void ASBaseWeapon::BeginPlay()
{
	Super::BeginPlay();
	TimeBetweenShots = 60 / RateOfFire;
	CurrentAmmo = AmmoPerMag;
}

void ASBaseWeapon::Fire()
{
	if (bOutOfAmmo == false) {
		if (Role < ROLE_Authority) {
			ServerFire();
		}

		if (CurrentAmmo > 0 && bOutOfAmmo == false)
			CurrentAmmo--;
		else {
			bOutOfAmmo = true;
			UE_LOG(LogTemp, Warning, TEXT("Out of ammo"));
		}
		// Get owner of the weapon
		AActor* MyPawn = GetOwner();
		if (MyPawn) {
			FVector EyeLocation; // Camera location
			FRotator EyeRotation; // Camera rotation
			MyPawn->GetActorEyesViewPoint(EyeLocation, EyeRotation); // Return value for camera's transform
			APlayerController* PC = Cast<APlayerController>(MyPawn->GetInstigatorController()); // Player controller
			if (PC)
				PC->ClientPlayCameraShake(CamShake);
			FVector ShotDirection = EyeRotation.Vector();
			FVector TraceEnd = EyeLocation + (ShotDirection * BulletMaxDistance); // End location of bullet

																				  // Apply recoil to weapon
			PC->AddPitchInput(RecoilY);
			PC->AddYawInput(RecoilX);

			FCollisionQueryParams QueryParam;
			QueryParam.AddIgnoredActor(MyPawn);
			QueryParam.AddIgnoredActor(this);
			QueryParam.bTraceComplex = true; // Trace into object triangles instead of collision box
			QueryParam.bReturnPhysicalMaterial = true;
			FVector TracerEndPoint = TraceEnd;
			FHitResult Hit;

			EPhysicalSurface SurfaceType = SurfaceType_Default; // Player hit surface type
			if (GetWorld()->LineTraceSingleByChannel(Hit, EyeLocation, TraceEnd, ECollisionChannel::COLLISION_WEAPON, QueryParam)) { // If bullet/linetrace hit something
				AActor* HitActor = Hit.GetActor(); // Get hit actor
												   /*if (DebugWeaponLineTrace > 0)
												   DrawDebugLine(GetWorld(), EyeLocation, TraceEnd, FColor::Red, false, 1.0f, 0, 1.0f); // Draw debug if debug is enabled through console*/
				SurfaceType = UPhysicalMaterial::DetermineSurfaceType(Hit.PhysMaterial.Get()); // Get hit surface type
				float ActualDamage = BaseDamage;
				if (SurfaceType == SURFACE_FLESHVULNERABLE) // If bullet hit the head
					ActualDamage *= 5.0f; // Damage multiply
				UGameplayStatics::ApplyPointDamage(HitActor, ActualDamage, ShotDirection, Hit, MyPawn->GetInstigatorController(), this, DamageType); // Apply damage to hit point
				PlayImpactEffect(SurfaceType, Hit.ImpactPoint);

				TracerEndPoint = Hit.ImpactPoint; // Set trace end point to hit impact point
				FireEffect(TracerEndPoint); // Impact hit effect (non-pawn objects)
				if (Role == ROLE_Authority) {
					HitScanTrace.TraceTo = TracerEndPoint;
					HitScanTrace.SurfaceType = SurfaceType;
				}
				LastFireTime = GetWorld()->TimeSeconds;
				if (FireSound)
					UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
			}
		}
	}
	
}

void ASBaseWeapon::ServerFire_Implementation() 
{
	Fire();
}

bool ASBaseWeapon::ServerFire_Validate()
{
	return true;
}

void ASBaseWeapon::StartFire()
{
	float FirstDelay = FMath::Max(LastFireTime + TimeBetweenShots - GetWorld()->TimeSeconds, 0.0f);
	GetWorldTimerManager().SetTimer(TimerHandle, this, &ASBaseWeapon::Fire, TimeBetweenShots, true, FirstDelay);
}

void ASBaseWeapon::StopFire()
{
	GetWorldTimerManager().ClearTimer(TimerHandle);
}

void ASBaseWeapon::PlayImpactEffect(EPhysicalSurface SurfaceType, FVector ImpactPoint)
{
	UParticleSystem* SelectedEffect = nullptr;
	switch (SurfaceType) { // Switch effect depending on the hit surface
		case SURFACE_FLESHDEFAULT:
		case SURFACE_FLESHVULNERABLE:
			SelectedEffect = FleshImpactEffect;
			break;
		default:
			SelectedEffect = ImpactEffect;
	}
	if (SelectedEffect) { // Play the selected effect
		FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleName);
		FVector ShotDirection = ImpactPoint - MuzzleLocation;
		ShotDirection.Normalize();
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SelectedEffect, ImpactPoint, ShotDirection.Rotation());
	}
}

void ASBaseWeapon::FireEffect(FVector TracerEndPoint)
{
	if (MuzzleSplash != nullptr) {
		UGameplayStatics::SpawnEmitterAttached(MuzzleSplash, MeshComp, MuzzleName); // Spawn muzzlesplash at socket muzzlesplash
	}
	if (SmokeTrail != nullptr) {
		FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleName);
		UParticleSystemComponent* SmokeTrailComp = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SmokeTrail, MuzzleLocation);
		if (SmokeTrailComp) {
			SmokeTrailComp->SetVectorParameter("BeamEnd", TracerEndPoint);
		}
	}
}



void ASBaseWeapon::Reload()
{
	if (Role < ROLE_Authority) {
		ServerReload();
	}
	if (CurrentAmmo == 0 && TotalAmmo >= AmmoPerMag) {
		CurrentAmmo = AmmoPerMag;
		TotalAmmo -= AmmoPerMag;
		bOutOfAmmo = false;
	}
	else if (CurrentAmmo == 0 && TotalAmmo > 0 && TotalAmmo < AmmoPerMag) {
		CurrentAmmo = TotalAmmo;
		TotalAmmo = 0;
		bOutOfAmmo = false;
	}
	else if (CurrentAmmo != AmmoPerMag){
		int32 DepletedAmmo = AmmoPerMag - CurrentAmmo;
		CurrentAmmo = AmmoPerMag;
		TotalAmmo -= DepletedAmmo;
		bOutOfAmmo = false;
	}
	else return;
}


void ASBaseWeapon::ServerReload_Implementation()
{
	Reload();
}

bool ASBaseWeapon::ServerReload_Validate()
{
	return true;
}

void ASBaseWeapon::OnRep_HitScanTrace()
{
	// Play cosmetic effect
	FireEffect(HitScanTrace.TraceTo);
	PlayImpactEffect(HitScanTrace.SurfaceType, HitScanTrace.TraceTo);
}
// Called every frame
void ASBaseWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASBaseWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ASBaseWeapon, HitScanTrace, COND_SkipOwner);
}