// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "pn_attackProjectile.h"
#include "char_Unit.h"

#include "CoreMinimal.h"
#include "AIController.h"
#include "AC_attackProjectileController.generated.h"

/**
 * 
 */
//Forward Declaration


UCLASS()
class MOBAGAME_API AAC_attackProjectileController : public AAIController
{
	GENERATED_BODY()
	


public:
	AAC_attackProjectileController();
	UFUNCTION(Server, Reliable, WithValidation) void moveToTarget();
	virtual void Tick(float DeltaTime) override;
};
