// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "Components/SphereComponent.h"
#include "Net/UnrealNetwork.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "CoreMinimal.h"
#include "char_BaseChampion.h"
#include "champ_WaoPierre.generated.h"

/**
 * 
 */
UCLASS()
class MOBAGAME_API Achamp_WaoPierre : public Achar_BaseChampion
{
	GENERATED_BODY()
	

public:
	
	UPROPERTY(EditAnywhere) TSubclassOf<UNiagaraSystem> FireSystem;
	UPROPERTY(EditAnywhere) UNiagaraSystem* FireComponent;
	UPROPERTY(EditAnywhere) UNiagaraComponent* Fire;
	UPROPERTY(EditAnywhere) UStaticMeshComponent* FireSitIn;
	Achamp_WaoPierre();
	virtual void Tick(float DeltaTime) override;
	
	void receiveDamage(float val) override;
	void ability_1() override;
	void ability_1_Animation() override;
	void ability_2() override;
	USphereComponent* FireBall;
	
	//virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps)const override;
	UFUNCTION() void end_fire();
	
protected:
	virtual void BeginPlay() override;
private:
	float staggeredDamage = 0.f;
	bool Ability1Active = false;
	FTimerHandle FireHandle;
	
};
