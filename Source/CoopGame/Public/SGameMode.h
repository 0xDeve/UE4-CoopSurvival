// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SGameMode.generated.h"

enum class EWaveState : uint8;
class USHealthComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnActorKilled, AActor*, VictimActor, AActor*, KillerActor, AController*, KillerController); // Killed actor, killer actor, 
/**
 * 
 */
UCLASS()
class COOPGAME_API ASGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
protected:
	// Hook for BP to spawn a single bot
	UFUNCTION(BlueprintImplementableEvent, Category="GameMode")
	void SpawnNewBot();

	// Start spawning bots
	void StartWave();
	
	// Stop spawning bots
	void EndWave();

	// Set timer for next start wave
	void PrepareForNextWave();

	void SpawnBotTimerElapsed();
	FTimerHandle TimerHandle_BotSpawner;
	FTimerHandle TimerHandle_NextWaveStart;
	// Bots to spawn in a single wave
	int32 NrOfBotsToSpawn;

	UPROPERTY(Replicated,BlueprintReadOnly, Category="Waves")
	int32 WaveCount;

	UPROPERTY(EditDefaultsOnly, Category="GameMode")
	float TimeBetweenWave;

	void CheckWaveState();

	void CheckIfAnyPlayerAlive();

	void GameOver();

	void SetWaveState(EWaveState NewState);
public:
	ASGameMode();
	virtual void StartPlay() override;

	virtual void Tick(float DeltaSeconds) override;
	UPROPERTY(BlueprintAssignable, Category="GameMode")
	FOnActorKilled OnActorKilled;
private:

};
