// Fill out your copyright notice in the Description page of Project Settings.


#include "W_ChampSelect.h"

#include "Components/Button.h"

void UW_ChampSelect::SpawnChampion()
{
	if (controllerRef)
	{
		controllerRef->CreatePlayer();
	}
}


