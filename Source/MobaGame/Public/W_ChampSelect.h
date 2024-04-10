// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../PC_ChampController.h"

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "W_ChampSelect.generated.h"

/**
 * 
 */
UCLASS()
class MOBAGAME_API UW_ChampSelect : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable) void SpawnChampion();
	UPROPERTY(EditAnywhere,BlueprintReadWrite)FString chosenChamp = "Wao Pierre";
	APC_ChampController* controllerRef = nullptr;

	



};
