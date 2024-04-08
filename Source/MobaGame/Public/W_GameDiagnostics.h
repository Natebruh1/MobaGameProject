// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "../AC_PlayerAIController.h"
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "W_GameDiagnostics.generated.h"

/**
 * 
 */
UCLASS()
class MOBAGAME_API UW_GameDiagnostics : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere) FText ActorName = FText::FromString("ActorName");
	//From PlayerState & System
	UPROPERTY(BlueprintReadWrite, EditAnywhere) int playerID;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) int pingCount;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) float fps;
	//From AIController
	UPROPERTY(BlueprintReadWrite, EditAnywhere) FVector cachedMoveLocation;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) TEnumAsByte<EInternalActionState> currentState = Idle;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) FVector targetVector;

	//From Player
	UPROPERTY(BlueprintReadWrite, EditAnywhere) bool unitTargetable;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) bool unitVisible;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) float totalAttack;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) float currentHealth;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) float unitSize;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) float secondaryResourcePercentage;
	UPROPERTY(BlueprintReadWrite,EditAnywhere) Achar_Unit* targetedUnit=nullptr;
	//From RPC
	UPROPERTY(BlueprintReadWrite, EditAnywhere) FText CurrentNetworkAction = FText::FromString("CurrentNetworkAction");
};
