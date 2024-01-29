// Fill out your copyright notice in the Description page of Project Settings.


#include "AC_attackProjectileController.h"

AAC_attackProjectileController::AAC_attackProjectileController()
{
	bReplicates = true;
	PrimaryActorTick.bCanEverTick = true; //Enable ticking on this ai controller
	SetActorTickEnabled(true);
	PrimaryActorTick.bStartWithTickEnabled = true;
}

void AAC_attackProjectileController::moveToTarget_Implementation()
{
	if (GetPawn<Apn_attackProjectile>()->getTargetUnit()) //If we can find the targeted unit reference
	{
		//UE_LOG(LogTemp, Warning, TEXT("Projectile Moving cuase target found"));
		MoveToActor(GetPawn<Apn_attackProjectile>()->getTargetUnit()); //Move towards the targeted unit
		
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Projectile not Moving cuase target not found")); //Otherwise display a warning that we can't find the target
	}
	
	
}
bool AAC_attackProjectileController::moveToTarget_Validate() { return true; }


void AAC_attackProjectileController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	moveToTarget();
	;
}
