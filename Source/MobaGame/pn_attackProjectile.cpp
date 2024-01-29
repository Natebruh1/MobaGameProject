// Fill out your copyright notice in the Description page of Project Settings.


#include "pn_attackProjectile.h"
#include "char_Unit.h"


// Sets default values
Apn_attackProjectile::Apn_attackProjectile()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	bAlwaysRelevant = true;
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	ProjectileMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Projectile Mesh"));
	ProjectileMeshComponent->SetupAttachment(RootComponent);
	FloatingMovementComponent = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("Flying Movement Component"));
	
	SetActorEnableCollision(false);
	
	
}

// Called when the game starts or when spawned
void Apn_attackProjectile::BeginPlay()
{
	Super::BeginPlay();
	SetReplicates(true);
	SetReplicateMovement(true);
}

// Called every frame
void Apn_attackProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (this->getTargetUnit()) //Check we have a target unit
	{
		float distToTarget = (this->getTargetUnit()->GetActorLocation() - this->GetActorLocation()).Size();
		if (distToTarget < 130.f)//If within distance
		{
			UE_LOG(LogTemp, Warning, TEXT("Destroying Projectile")); //Otherwise display a warning that we can't find the target
			getTargetUnit()->receiveDamage(300.f); //Need to update with the caller units attack damage
			this->Destroy();
		}
	}
	else
	{
		Destroy();
	}
	

}

// Called to bind functionality to input
void Apn_attackProjectile::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

Achar_Unit* Apn_attackProjectile::getTargetUnit()
{
	return targetUnit;
}

void Apn_attackProjectile::setTargetUnit_Implementation(Achar_Unit* newTarget)
{
	targetUnit = newTarget;
}
bool Apn_attackProjectile::setTargetUnit_Validate(Achar_Unit* newTarget) { return true; }



void Apn_attackProjectile::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps)const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(Apn_attackProjectile, targetUnit);
	

	//DOREPLIFETIME_CONDITION(APC_ChampController, cameraAttached, COND_OwnerOnly);
}