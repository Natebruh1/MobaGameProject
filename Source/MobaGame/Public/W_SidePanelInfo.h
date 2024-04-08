// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "W_SidePanelInfo.generated.h"

/**
 * 
 */
UCLASS()
class MOBAGAME_API UW_SidePanelInfo : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere) float attack;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) FText qCooldown;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) FText wCooldown;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) FText eCooldown;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) float secondaryResourcePercentage;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) float healthPercentage;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) FText secondaryName=FText::FromString("Mana");
	UPROPERTY(BlueprintReadWrite, EditAnywhere) FText QDesc = FText::FromString("Q Description");
	UPROPERTY(BlueprintReadWrite, EditAnywhere) FText WDesc = FText::FromString("W Description");
	UPROPERTY(BlueprintReadWrite, EditAnywhere) FText EDesc = FText::FromString("E Description");
};
