// Fill out your copyright notice in the Description page of Project Settings.

#include "GrenadeLauncher.h"
#include "GrenadeProjectile.h"

void AGrenadeLauncher::Fire()
{
	Super::Fire();
	if (ProjectileClass)
	{
		FTransform MuzzleTransform = MeshComp->GetSocketTransform("muzzle splash");
		
		//Set Spawn Collision Handling Override
		FActorSpawnParameters ActorSpawnParams;
		ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		
		AActor* MyPawn = GetOwner();
		FVector EyeLocation;
		FRotator EyeRotation;
		MyPawn->GetActorEyesViewPoint(EyeLocation, EyeRotation);
		FVector ShotDirection = EyeRotation.Vector();
		FVector TraceStart = EyeLocation + (ShotDirection * 100);

		// spawn the projectile at the muzzle
		GetWorld()->SpawnActor<AGrenadeProjectile>(ProjectileClass, TraceStart ,EyeRotation, ActorSpawnParams);
	}
}

