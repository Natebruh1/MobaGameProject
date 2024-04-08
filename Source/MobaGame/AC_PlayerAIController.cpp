// Fill out your copyright notice in the Description page of Project Settings.


#include "AC_PlayerAIController.h"

AAC_PlayerAIController::AAC_PlayerAIController()
{
	bReplicates = true;
	PrimaryActorTick.bCanEverTick = true;
	SetActorTickEnabled(true);
	PrimaryActorTick.bStartWithTickEnabled = true;
}

void AAC_PlayerAIController::ServerSetTargetUnit_Implementation(Achar_Unit* targetUnitVal)
{
	//UE_LOG(LogTemp, Warning, TEXT("Changing Target to : %s"), *(targetUnitVal->getUnitName()));
	if (targetUnitVal == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("No unit found"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Unit found to target"));
		if (targetUnitVal->getUnitVisible() && targetUnitVal->getUnitTargetable())//Make sure the target is targetable and visible
		{
			GetPawn<Achar_Unit>()->setTargetUnit(targetUnitVal);
		}
		else
		{
			GetPawn<Achar_Unit>()->setTargetUnit(nullptr);
		}
	}
	
	
	
}
bool AAC_PlayerAIController::ServerSetTargetUnit_Validate(Achar_Unit* targetUnitVal) { return true; }

void AAC_PlayerAIController::swapTeams_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("player ai controller calling swap teams"));
	if (GetPawn<Achar_Unit>()->getUnitTeam() == Red) {
		GetPawn<Achar_Unit>()->joinTeam(Blue);


	}
	else if (GetPawn<Achar_Unit>()->getUnitTeam() == Blue) {
		GetPawn<Achar_Unit>()->joinTeam(Red);
	}
	
}
bool AAC_PlayerAIController::swapTeams_Validate() { return true; }



void AAC_PlayerAIController::Ability_1()
{
	GetPawn<Achar_BaseChampion>()->ability_1();
}



void AAC_PlayerAIController::Ability_1_Animation_Implementation()
{
	GetPawn<Achar_BaseChampion>()->ability_1_Animation();
}
bool AAC_PlayerAIController::Ability_1_Animation_Validate() { return true;  }

void AAC_PlayerAIController::BeginPlay()
{
	Super::BeginPlay();
	SetReplicates(true);
	SetReplicateMovement(true);

}

void AAC_PlayerAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (GetPawn<Achar_BaseChampion>())
	{
		GetPawn<Achar_BaseChampion>()->mouseVec = cachedMouseVec;
	}
	switch (currentState) {
	case MoveState:
		//UE_LOG(LogTemp, Warning, TEXT("MOVING CONTROLLED PLAYER"));
		MoveControlledPlayer(cachedMoveLocation);
		if ((GetPawn()->GetActorLocation() - cachedMoveLocation).Length() < 128)//Magic Number, must change later WIP
		{
			UE_LOG(LogTemp, Warning, TEXT("GOING IDLE"));
			currentState = Idle;
		}
		break;
	case AttackMove:
		targetVector = (GetPawn()->GetActorLocation()) - (GetPawn<Achar_Unit>()->getTargetUnit()->GetActorLocation());
		if (targetVector.Length() > GetPawn<Achar_Unit>()->getUnitRange()) {
			MoveToActor(GetPawn<Achar_Unit>()->getTargetUnit());
		}
		else {
			GetPawn()->GetMovementComponent()->StopActiveMovement();//stop us from moving when in range
			currentState = Attack;
		}
		break;
	case Attack:
		
		float remainingAttackWindup = GetWorldTimerManager().GetTimerRemaining(*GetPawn<Achar_Unit>()->getAttackTimerHandle());
		if (!(remainingAttackWindup > 0.f))//If we have no remaining attack timer
		{
			
			UE_LOG(LogTemp, Warning, TEXT("Attack available, starting attack windup"));
			GetPawn<Achar_Unit>()->windupAttack();//Start the windup for the attack
		}
		else
		{
			//UE_LOG(LogTemp, Warning, TEXT("Attack winding..."));
		}
		break;
	}
}

void AAC_PlayerAIController::updateInternalState(EInternalActionState newState)
{
	UE_LOG(LogTemp, Warning, TEXT("--UPDATING STATE--"));
	currentState = newState;
}

FVector AAC_PlayerAIController::getCachedMoveLocation()
{
	return cachedMoveLocation;
}

void AAC_PlayerAIController::setCachedMoveLocation(FVector val)
{
	UE_LOG(LogTemp, Warning, TEXT("--UPDATING CACHED LOCATION--"));
	cachedMoveLocation = val;
}

EInternalActionState AAC_PlayerAIController::getCurrentState()
{
	return currentState;
}

void AAC_PlayerAIController::MoveControlledPlayer_Implementation(FVector movePosition)
{
	//UE_LOG(LogTemp, Warning, TEXT("---Moving Player---> %s"),movePosition);
	MoveTo(movePosition);
}
bool AAC_PlayerAIController::MoveControlledPlayer_Validate(FVector movePosition) { return true; }

