// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "Components/SphereComponent.h"
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
	class UPROPERTY(EditAnywhere, Replicated) UNiagaraComponent* Fire;
	Achamp_WaoPierre();
	virtual void Tick(float DeltaTime) override;
	void receiveDamage(float val) override;
	void ability_1() override;
	void ability_1_Animation() override;
	USphereComponent* FireBall;

	//virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps)const override;
private:
	float staggeredDamage = 0.f;
	
};
