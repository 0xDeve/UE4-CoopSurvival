// Fill out your copyright notice in the Description page of Project Settings.

#include "SCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "SWeapon.h"
#include "CoopGame.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SHealthComponent.h"
#include "Net/UnrealNetwork.h"
#include "Weapons/SBaseWeapon.h"
#include "SGameInstance.h"
// Sets default values
ASCharacter::ASCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	ThirdPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Third person camera"));
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring arm component"));
	SpringArmComp->bUsePawnControlRotation = true;
	SpringArmComp->SetupAttachment(RootComponent);
	ThirdPersonCamera->SetupAttachment(SpringArmComp);
	ZoomFOV = 50.f;
	ZoomInterpSpeed = 20.0f;
	GetCapsuleComponent()->SetCollisionResponseToChannel(COLLISION_WEAPON, ECR_Ignore);
	HealthComp = CreateDefaultSubobject<USHealthComponent>(TEXT("Health component"));
	HealthComp->OnHealthChanged.AddDynamic(this, &ASCharacter::OnHealthChanged);
}

// Called when the game starts or when spawned
void ASCharacter::BeginPlay()
{
	Super::BeginPlay();
	USGameInstance* GI = Cast<USGameInstance>(GetWorld()->GetGameInstance());
	MainWeaponClass = GI->SelectedWeapons.FindOrAdd(0);
	GetMovementComponent()->GetNavAgentPropertiesRef().bCanCrouch = true;
	DefaultFOV = ThirdPersonCamera->FieldOfView;
	if (Role == ROLE_Authority) {
		// Spawn the weapon
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		CurrentWeapon = GetWorld()->SpawnActor<ASBaseWeapon>(MainWeaponClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
		if (CurrentWeapon) {
			CurrentWeapon->SetOwner(this);
			CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, WeaponAttachSocketName);
		}
	}
}

void ASCharacter::MoveForward(float Value)
{
	AddMovementInput(GetActorForwardVector() * Value);
}


void ASCharacter::MoveRight(float Value)
{
	AddMovementInput(GetActorRightVector() * Value);
}

void ASCharacter::BeginCrouch()
{
	if (bIsCrouching == false && GetCharacterMovement()->bWantsToCrouch == false) {
		bIsCrouching = true;
		GetCharacterMovement()->bWantsToCrouch = true;
	}
	else if (bIsCrouching == true && GetCharacterMovement()->bWantsToCrouch == true) {
		bIsCrouching = false;
		GetCharacterMovement()->bWantsToCrouch = false;
	}
}



void ASCharacter::BeginZoom()
{
	bWantsToZoom = true;
}

void ASCharacter::EndZoom()
{
	bWantsToZoom = false;
}

void ASCharacter::StartFire()
{
	CurrentWeapon->Fire();
	CurrentWeapon->StartFire();
	bIsShooting = true;
}

void ASCharacter::StopFire() 
{
	CurrentWeapon->StopFire();
	bIsShooting = false;
}

void ASCharacter::StartSprint()
{
	bIsSprinting = true;
	GetCharacterMovement()->MaxWalkSpeed = 1200.f;
}

void ASCharacter::StopSprint()
{
	bIsSprinting = false;
	GetCharacterMovement()->MaxWalkSpeed = 600.f;
}

void ASCharacter::StartProne()
{
	bIsProning = true;
}

void ASCharacter::StopProne()
{

}

void ASCharacter::OnHealthChanged(USHealthComponent* OwningHealthComp, float Health, float HealthData, const class UDamageType* DamageType, 
	class AController* InstigatedBy, AActor* DamageCauser)
{
	if (Health <= 0.0f && !bDied) {
		bDied = true;
		GetMovementComponent()->StopMovementImmediately();
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		DetachFromControllerPendingDestroy();

		SetLifeSpan(4.f);
	}
}

void ASCharacter::StartReload()
{
	CurrentWeapon->Reload();
}

// Called every frame
void ASCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	float TargetFOV = bWantsToZoom ? CurrentWeapon->ZoomFOV : DefaultFOV;
	float CurrentFOV = FMath::FInterpTo(ThirdPersonCamera->FieldOfView, TargetFOV, DeltaTime, ZoomInterpSpeed);
	ThirdPersonCamera->SetFieldOfView(CurrentFOV);
	
}

// Called to bind functionality to input
void ASCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &ASCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ASCharacter::MoveRight);
	PlayerInputComponent->BindAxis("LookUp", this, &ASCharacter::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Turn", this, &ASCharacter::AddControllerYawInput);
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &ASCharacter::BeginCrouch);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Zoom", IE_Pressed, this, &ASCharacter::BeginZoom);
	PlayerInputComponent->BindAction("Zoom", IE_Released, this, &ASCharacter::EndZoom);
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ASCharacter::StartFire);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &ASCharacter::StopFire);
	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &ASCharacter::StartSprint);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &ASCharacter::StopSprint);
	PlayerInputComponent->BindAction("Prone", IE_Pressed, this, &ASCharacter::StartProne);
	PlayerInputComponent->BindAction("Prone", IE_Released, this, &ASCharacter::StopProne);
	PlayerInputComponent->BindAction("Reload", IE_Released, this, &ASCharacter::StartReload);


}

FVector ASCharacter::GetPawnViewLocation() const
{
	if (ThirdPersonCamera == nullptr) return Super::GetPawnViewLocation();
	return ThirdPersonCamera->GetComponentLocation();
}

void ASCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASCharacter, CurrentWeapon);
	DOREPLIFETIME(ASCharacter, bDied);
}