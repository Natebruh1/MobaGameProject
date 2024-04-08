// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "playerCharState.generated.h"

/**
 * 
 */
UCLASS()
class MOBAGAME_API AplayerCharState : public APlayerState
{
	GENERATED_BODY()
	
public:
	float cd1 = 0.f;
	float cd2 = 0.f;
	float cd3 = 0.f;
};
