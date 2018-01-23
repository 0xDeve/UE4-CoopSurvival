// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SAmmoBox.generated.h"

class UBoxComponent;
UCLASS()
class COOPGAME_API ASAmmoBox : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASAmmoBox();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UPROPERTY(VisibleAnywhere, Category = "Components")
		UStaticMeshComponent* MeshComp;
	UPROPERTY(VisibleAnywhere, Category = "Components")
		UBoxComponent* CollisionComp;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
	
};
