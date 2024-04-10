// Fill out your copyright notice in the Description page of Project Settings.


#include "champ_Lucien.h"

Achamp_Lucien::Achamp_Lucien()
{
	chosenChampion = FString("Lucien");
	//Set base stats
	Ability1CD = 8.f;
	Ability3CD = 12.f;
	setUnitAttackRange(900.f);
	baseAttack = 300.f;

	secondaryName = FText::FromString("Mana");
	Ability1Desc = FText::FromString("Your passively gain attack damage when close to your target. Activate to dash.");
	Ability2Desc = FText::FromString("Slash around you dealing damage | WIP");
	Ability3Desc = FText::FromString("Knockback Nearby Enemies");
}

void Achamp_Lucien::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (getTargetUnit())
	{
		FVector distVec = getTargetUnit()->GetActorLocation() - GetActorLocation();
		float dist = distVec.Size()/10.f;
		baseAttack = 300.0 + ((500.f+bonusAttack) / max(dist, 1.0));
	}
	

	secondaryPercentage = mana / maxMana;
}


void Achamp_Lucien::ability_1()
{
	UE_LOG(LogTemp, Warning, TEXT("Lucien Ability 1"));

	//FireSitIn->SetWorldScale3D(FVector(1.f, 1.f, 2.f));


	//FireBall->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

	if (!GetWorldTimerManager().IsTimerActive(Ability1Handle))
	{
		if (mana > 80.f)//Mana Cost
		{
			
			mana -= 80.f;
			FVector launchVec = mouseVec+FVector(0.f,0.f,30.f);
			launchVec.Normalize();
			launchVec = launchVec*10000.f;
			GetMovementComponent()->StopActiveMovement();
			LaunchCharacter(launchVec,true,true);
			
			GetWorldTimerManager().SetTimer(Ability1Handle, Ability1CD, false);
		}

	}
}

void Achamp_Lucien::ability_2()
{
}

void Achamp_Lucien::ability_3()
{
}

void Achamp_Lucien::BeginPlay()
{
	Super::BeginPlay();
}
