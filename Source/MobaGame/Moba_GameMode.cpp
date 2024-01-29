// Fill out your copyright notice in the Description page of Project Settings.


#include "Moba_GameMode.h"
#include "Moba_GameState.h"
AMoba_GameMode::AMoba_GameMode()
{
	
	
}

void AMoba_GameMode::RemoveUnitFromTeam(Achar_Unit* unitToRemove, TeamName team)
{
	if (unitToRemove)
	{
		if (GetWorld()->GetGameState<AMoba_GameState>())
		{
			GetWorld()->GetGameState<AMoba_GameState>()->getTeamArray(team)->Remove(unitToRemove);
			UE_LOG(LogTemp, Warning, TEXT("Removing unit via gamemode"));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("GameState not found"));
		}
		
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Unable to remove unit via gamemode"));
	}
	
}

void AMoba_GameMode::AddUnitToTeam(Achar_Unit* unitToAdd, TeamName team)
{
	GetWorld()->GetGameState<AMoba_GameState>()->getTeamArray(team)->Add(unitToAdd);
	UE_LOG(LogTemp, Warning, TEXT("Adding unit via gamemode"));
}
