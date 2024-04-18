// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "./Public/pn_baseAimedProjectile.h"
#include "CoreMinimal.h"
#include "char_BaseChampion.h"
#include "champ_Helena.generated.h"

/**
 * 
 */
class AAC_PlayerAIController;

UCLASS()
class MOBAGAME_API Achamp_Helena : public Achar_BaseChampion
{
	GENERATED_BODY()
public:
	Achamp_Helena();
	virtual void Tick(float DeltaTime) override;
	void ability_1() override;

	void ability_2() override;
	void ability_3() override;

	void attackUnit() override;

	UPROPERTY(EditAnywhere) USphereComponent* Slash;
	virtual void BeginPlay() override;
	void ScriptDashDirection(AAC_PlayerAIController* args);

	UPROPERTY(EditAnywhere)TSubclassOf<Apn_baseAimedProjectile> AbilityProjectile;
	UPROPERTY(EditAnywhere) USphereComponent* HealAura;
private: 
	bool Ability3Active = false;
	FVector lockedInFloatPos = FVector::Zero();
	FRotator lockedInRot = FRotator::ZeroRotator;
	float dashSpeed = 100.f;
	float oldMoveSpeed = 600.f;
};
