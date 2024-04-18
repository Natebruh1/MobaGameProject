// Fill out your copyright notice in the Description page of Project Settings.


#include "pn_baseAimedProjectile.h"
#include "../char_Unit.h"
// Sets default values
Apn_baseAimedProjectile::Apn_baseAimedProjectile()
{
	bReplicates = true;
	bAlwaysRelevant = true;
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	projHitbox = CreateDefaultSubobject<USphereComponent>(TEXT("Projectile Collision")); //For overlapping
	projHitbox->SetupAttachment(GetRootComponent());
	projHitbox->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Overlap);
	projHitbox->SetSphereRadius(64.f);

	ProjectileMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Projectile Mesh"));
	ProjectileMeshComponent->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void Apn_baseAimedProjectile::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void Apn_baseAimedProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	FVector increase = GetActorLocation() + (baseDir.Normalize()) * projSpeed * DeltaTime;
	SetActorLocation(increase);
	currentLength += projSpeed * DeltaTime;
	//SetActorLocation(GetActorLocation()+FVector(0.f,0.f,50.f*DeltaTime));
	if (currentLength > totalLength)
	{
		Destroy();
	}

	TArray<AActor*> projOverlap;
	projHitbox->GetOverlappingActors(projOverlap, Achar_Unit::StaticClass()); //Get the actors
	for (int i = 0; i < projOverlap.Num(); i++)
	{
		if (projOverlap[i] != this && Cast<Achar_Unit>(projOverlap[i])->getUnitTeam() != team) //Cast the overlapping actor and check its on the opposite team
		{
			Achar_Unit* hitTarg = Cast<Achar_Unit>(projOverlap[i]); //If it is then deal damage to that target
			
			hitTarg->receiveDamage(totalDamage); 
			Destroy();

		}
	}
}

void Apn_baseAimedProjectile::collisionEffect()
{
}

