// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LaunchPad.generated.h"

class UBoxComponent;

UCLASS()
class COOPGAME_API ALaunchPad : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALaunchPad();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UPROPERTY(VisibleAnywhere, Category = "LaunchPad")
		UStaticMeshComponent* MeshComp;
	UPROPERTY(VisibleAnywhere, Category = "LaunchPad")
		UBoxComponent* BoxComp;
	UPROPERTY(EditInstanceOnly, Category = "LaunchForce")
		FVector LaunchForce;
	UFUNCTION()
		void LaunchOverlappedObject(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
	
};
