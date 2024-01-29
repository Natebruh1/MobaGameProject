// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "char_Unit.h"
#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "Moba_GameState.generated.h"








/**
 * 
 */
UCLASS()
class MOBAGAME_API AMoba_GameState : public AGameState
{
	GENERATED_BODY()
public:
	TArray<Achar_Unit*>* getTeamArray(TeamName team);
	void RemoveUnitFromTeam(Achar_Unit* unitToRemove, TeamName team);
	void AddUnitToTeam(Achar_Unit* unitToAdd, TeamName team);

	int getPlayerCount();
	void setPlayerCount(int newCount);
private:
	//3 teams, red, blue, neutral
	TArray<Achar_Unit*>* redTeam =new(TArray<Achar_Unit*>);
	TArray<Achar_Unit*>* blueTeam = new(TArray<Achar_Unit*>);
	TArray<Achar_Unit*>* neutralTeam = new(TArray<Achar_Unit*>);
	int playerCount = 0;
	
};
