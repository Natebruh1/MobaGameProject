// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "pn_baseAimedProjectile.generated.h"

UCLASS()
class MOBAGAME_API Apn_baseAimedProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	Apn_baseAimedProjectile();
	UPROPERTY(EditAnywhere) float totalLength=1800.f;
	UPROPERTY(EditAnywhere) float currentLength = 0.f;
	UPROPERTY(EditAnywhere) float projSpeed = 600.f;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void collisionEffect();
	UPROPERTY(EditAnywhere) USphereComponent* projHitbox;
	TEnumAsByte<TeamName> team;
	UPROPERTY(EditAnywhere) float totalDamage = 50.f;
	UPROPERTY(EditAnywhere) class UStaticMeshComponent* ProjectileMeshComponent;
	FVector baseDir = FVector::Zero();

};
