// Fill out your copyright notice in the Description page of Project Settings.

#include "SAmmoBox.h"
#include "Components/BoxComponent.h"

// Sets default values
ASAmmoBox::ASAmmoBox()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = MeshComp;
	CollisionComp = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision box"));
	CollisionComp->SetupAttachment(MeshComp);
}

// Called when the game starts or when spawned
void ASAmmoBox::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASAmmoBox::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

