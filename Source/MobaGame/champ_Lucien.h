// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "char_BaseChampion.h"
#include "champ_Lucien.generated.h"

/**
 * 
 */
UCLASS()
class MOBAGAME_API Achamp_Lucien : public Achar_BaseChampion
{
	GENERATED_BODY()
public:
	Achamp_Lucien();
	virtual void Tick(float DeltaTime) override;
	void ability_1() override;
	
	void ability_2() override;
	void ability_3() override;
protected:
	virtual void BeginPlay() override;
};
