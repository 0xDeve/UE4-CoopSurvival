// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "SGameState.generated.h"

UENUM(BlueprintType)
enum class EWaveState : uint8 {
	WaitingToStart,
	PreparingNextWave,
	WaveInProgress,
	// No longer spawning new bots, waiting for players to kill remaining bots
	WaitingToComplete,
	WaveComplete,
	GameOver,
};
/**
 * 
 */

class ASBaseWeapon;

UCLASS()
class COOPGAME_API ASGameState : public AGameStateBase
{
	GENERATED_BODY()
protected:

	UFUNCTION()
	void OnRep_WaveState(EWaveState OldState);
	UFUNCTION(BlueprintImplementableEvent, Category="GameState")
	void WaveStateChanged(EWaveState NewState, EWaveState OldState);
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_WaveState, Category = "GameState")
		EWaveState WaveState;
public:

	void SetWaveState(EWaveState NewState);
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
		TMap<int32, TSubclassOf<ASBaseWeapon>> SelectedWeapons;
	UFUNCTION(BlueprintCallable, Category = "Weapon")
		void AddSelectedWeapons(TSubclassOf<ASBaseWeapon> SelectedWeapon, int32 PlayerID);
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "GameState")
		int32 WaveCount;
	
};
