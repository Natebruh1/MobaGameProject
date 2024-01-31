// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "char_BaseChampion.h"
#include "CoreMinimal.h"
#include "AIController.h"
#include "AC_PlayerAIController.generated.h"


/**
 * 
 */
UENUM() enum EInternalActionState
{
	MoveState UMETA(DisplayName = "Moving to cached location"),
	AttackMove UMETA(DisplayName = "Moving towards unit to attack when in range"),
	Attack UMETA(DisplayName = "Attacking a unit in range"),
	Idle UMETA(DisplayName="Not performing any actions"),
	Channeling UMETA(DisplayName="Channeling ability or action"),
};


UCLASS()
class MOBAGAME_API AAC_PlayerAIController : public AAIController
{
	GENERATED_BODY()
public:
	AAC_PlayerAIController();
	UFUNCTION(Server, Reliable, WithValidation) void MoveControlledPlayer(FVector movePosition);
	UFUNCTION(Server, Reliable, WithValidation) void ServerSetTargetUnit(Achar_Unit* targetUnitVal);
	UFUNCTION(Server, Reliable, WithValidation) void swapTeams();
	void Ability_1();
	UFUNCTION(NetMulticast,Reliable,WithValidation) void Ability_1_Animation();
	virtual void BeginPlay();
	virtual void Tick(float DeltaTime) override;
	void updateInternalState(EInternalActionState newState);
	FVector getCachedMoveLocation();
	void setCachedMoveLocation(FVector val);
private:
	UPROPERTY(EditAnywhere) TEnumAsByte<EInternalActionState> currentState = Idle;
	UPROPERTY(EditAnywhere) FVector cachedMoveLocation;
	FVector targetVector;
	
};
