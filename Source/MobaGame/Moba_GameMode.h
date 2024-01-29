// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "char_Unit.h"

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "Moba_GameMode.generated.h"

/**
 * 
 */




UCLASS()
class MOBAGAME_API AMoba_GameMode : public AGameMode
{
	GENERATED_BODY()
public:
	AMoba_GameMode();
	
	void RemoveUnitFromTeam(Achar_Unit* unitToRemove, TeamName team);
	void AddUnitToTeam(Achar_Unit* unitToAdd, TeamName team);
	//UFUNCTION() AddToTeam
private:
	
};
