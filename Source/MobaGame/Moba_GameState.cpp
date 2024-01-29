// Fill out your copyright notice in the Description page of Project Settings.


#include "Moba_GameState.h"

TArray<Achar_Unit*>* AMoba_GameState::getTeamArray(TeamName team)
{
    if (this)
    {
        switch (team) {
        case Red:
            return redTeam;
            break;
        case Blue:
            return blueTeam;
            break;
        case Neutral:
            return neutralTeam;
            break;
        default:
            return nullptr;
            break;
        }
    }
    return nullptr;
}

void AMoba_GameState::RemoveUnitFromTeam(Achar_Unit* unitToRemove, TeamName team)
{
    if (HasAuthority()) {
        UE_LOG(LogTemp, Warning, TEXT("We have auth on game state"));
    }
    if (unitToRemove)
    {
        if (GetWorld()->GetGameState<AMoba_GameState>())
        {
            GetWorld()->GetGameState<AMoba_GameState>()->getTeamArray(team)->Remove(unitToRemove);
            UE_LOG(LogTemp, Warning, TEXT("Removing unit via gamestate"));
            unitToRemove->setUnitTeam(Neutral);
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("GameState not found"));
        }

    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Unable to remove unit via gamestate"));
    }
}

void AMoba_GameState::AddUnitToTeam(Achar_Unit* unitToAdd, TeamName team)
{
    GetWorld()->GetGameState<AMoba_GameState>()->getTeamArray(team)->Add(unitToAdd);
    UE_LOG(LogTemp, Warning, TEXT("Adding unit via gamestate"));
    unitToAdd->setUnitTeam(team);
}

int AMoba_GameState::getPlayerCount()
{
    return playerCount;
}

void AMoba_GameState::setPlayerCount(int newCount)
{
    playerCount = newCount;
}
