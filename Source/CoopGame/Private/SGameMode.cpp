// Fill out your copyright notice in the Description page of Project Settings.

#include "SGameMode.h"
#include "Components/SHealthComponent.h"
#include "SGameState.h"
#include "SPlayerState.h"
#include "Net/UnrealNetwork.h"

ASGameMode::ASGameMode()
{
	TimeBetweenWave = 2.0f;
	WaveCount = 1;
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 1.0f;
	GameStateClass = ASGameState::StaticClass();
	PlayerStateClass = ASPlayerState::StaticClass();
}



void ASGameMode::StartWave()
{
	NrOfBotsToSpawn = 2 * WaveCount;

	GetWorldTimerManager().SetTimer(TimerHandle_BotSpawner, this, &ASGameMode::SpawnBotTimerElapsed, 1.0f, true);
	SetWaveState(EWaveState::WaveInProgress);
}

void ASGameMode::EndWave()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_BotSpawner);
	SetWaveState(EWaveState::WaitingToComplete);

}

void ASGameMode::PrepareForNextWave()
{
	GetWorldTimerManager().SetTimer(TimerHandle_NextWaveStart, this, &ASGameMode::StartWave, TimeBetweenWave, false);
	SetWaveState(EWaveState::WaitingToStart);
}

void ASGameMode::SpawnBotTimerElapsed()
{
	SpawnNewBot();

	NrOfBotsToSpawn--;
	if (NrOfBotsToSpawn <= 0) {
		EndWave();
	}
}

void ASGameMode::CheckWaveState()
{
	bool bIsPreparingForWave = GetWorldTimerManager().IsTimerActive(TimerHandle_NextWaveStart);
	if (NrOfBotsToSpawn > 0 || bIsPreparingForWave) {
		return;
	}
	bool bIsAnyBotAlive = false;

	for (FConstPawnIterator It = GetWorld()->GetPawnIterator(); It; It++) {
		APawn* TestPawn = It->Get();
		if (TestPawn == nullptr || TestPawn->IsPlayerControlled()) {
			continue;
		}

		USHealthComponent* HealthComp = Cast<USHealthComponent>(TestPawn->GetComponentByClass(USHealthComponent::StaticClass()));
		if (HealthComp && HealthComp->GetHealth() > 0.0f) {
			bIsAnyBotAlive = true;
			break;
		}
	}
	ASGameState* GS = GetGameState<ASGameState>();
	if (!bIsAnyBotAlive) {
		SetWaveState(EWaveState::WaveComplete);
		GS->WaveCount++;
		WaveCount++;
		PrepareForNextWave();
	}
}

void ASGameMode::CheckIfAnyPlayerAlive()
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; It++) {
		APlayerController* PC = It->Get();
		if (PC && PC->GetPawn()) {
			APawn* MyPawn = PC->GetPawn();
			USHealthComponent* HealthComp =  Cast<USHealthComponent>(MyPawn->GetComponentByClass(USHealthComponent::StaticClass()));
			if (ensure(HealthComp) && HealthComp->GetHealth() > 0.0f) {
				return;
			}
		}
	}
	// End wave
	GameOver();
}

void ASGameMode::GameOver()
{
	EndWave();

	UE_LOG(LogTemp, Warning, TEXT("Game over"));
	SetWaveState(EWaveState::GameOver);
}

void ASGameMode::SetWaveState(EWaveState NewState)
{
	ASGameState* GS = GetGameState<ASGameState>();
	if (ensureAlways(GS)) {
		//GS->WaveState = NewState;
		GS->SetWaveState(NewState);
	}
}

void ASGameMode::StartPlay()
{
	Super::StartPlay();
	ASGameState* GS = GetGameState<ASGameState>();
	GS->WaveCount = WaveCount;
	PrepareForNextWave();
}

void ASGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	CheckWaveState();
	CheckIfAnyPlayerAlive();
}

void ASGameMode::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASGameMode, WaveCount);
}