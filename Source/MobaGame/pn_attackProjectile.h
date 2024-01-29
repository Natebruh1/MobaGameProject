// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "Components/StaticMeshComponent.h"
#include "GameFramework/FloatingPawnMovement.h"

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "pn_attackProjectile.generated.h"

class Achar_Unit;

UCLASS()
class MOBAGAME_API Apn_attackProjectile : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	Apn_attackProjectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	Achar_Unit* getTargetUnit();
	UFUNCTION(Server, Reliable, WithValidation) void setTargetUnit(Achar_Unit* newTarget);

private:
	UPROPERTY(VisibleAnywhere) class UStaticMeshComponent* ProjectileMeshComponent;
	UPROPERTY(VisibleAnywhere) class UFloatingPawnMovement* FloatingMovementComponent;
	UPROPERTY(EditAnywhere, Replicated) Achar_Unit* targetUnit;
};
