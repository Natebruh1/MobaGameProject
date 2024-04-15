// Fill out your copyright notice in the Description page of Project Settings.


#include "champ_Lucien.h"

Achamp_Lucien::Achamp_Lucien()
{
	chosenChampion = FString("Lucien");
	//Set base stats
	Ability1CD = 8.f;
	Ability3CD = 12.f;
	setUnitAttackRange(900.f);
	baseAttack = 190.f;
	startHealth = 1000.f; //Lower health than 1500 as this is ad carry
	currentHealth = 1000.f;

	//Slash attack and knockback overlap
	Slash = CreateDefaultSubobject<USphereComponent>(TEXT("Slash Sphere")); //For overlapping
	Slash->SetupAttachment(GetRootComponent());
	Slash->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Overlap);
	Slash->SetSphereRadius(320.f * baseSize);


	secondaryName = FText::FromString("Mana");
	Ability1Desc = FText::FromString("Your passively gain attack damage when close to your target. Activate to dash.");
	Ability2Desc = FText::FromString("Slash around you dealing damage");
	Ability3Desc = FText::FromString("Knockback Nearby Enemies");
}

void Achamp_Lucien::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (getTargetUnit())
	{
		FVector distVec = getTargetUnit()->GetActorLocation() - GetActorLocation();
		float dist = distVec.Size()/10.f;
		baseAttack = 190.0 + ((330.f+bonusAttack) / max(dist, 1.0)); //Increase AD by distance to closest target
	}
	
	Slash->SetWorldLocation(this->GetActorLocation()); //Set location to be slightly forwards
	secondaryPercentage = mana / maxMana;
}


void Achamp_Lucien::ability_1()
{
	UE_LOG(LogTemp, Warning, TEXT("Lucien Ability 1"));

	if (!GetWorldTimerManager().IsTimerActive(Ability1Handle)) //Not on cooldown
	{
		if (mana > 60.f)//Mana Cost
		{
			
			mana -= 60.f;
			FVector launchVec = (mouseVec*FVector(1.f,1.f,0.f))+FVector(0.f,0.f,30.f); //Get a vector to the player
			launchVec.Normalize(); //Normalize it to make sure movement is uniform
			launchVec = launchVec*2000.f; //Remultiply for a decent launch vector
			GetMovementComponent()->StopActiveMovement();
			LaunchCharacter(launchVec,false,false); //Launch this character
			
			GetWorldTimerManager().SetTimer(Ability1Handle, Ability1CD, false);
		}

	}
}

void Achamp_Lucien::ability_2()
{
	UE_LOG(LogTemp, Warning, TEXT("Lucien Ability 2"));

	if (!GetWorldTimerManager().IsTimerActive(Ability2Handle)) //Not on cooldown
	{
		if (mana > 100.f)//Mana Cost
		{

			mana -= 100.f;
			TArray<AActor*> SlashOverlap;
			Slash->GetOverlappingActors(SlashOverlap, Achar_Unit::StaticClass()); //Get the actors
			UE_LOG(LogTemp, Warning, TEXT("Slash Overlap : %d"), SlashOverlap.Num());
			for (int i = 0; i < SlashOverlap.Num(); i++)
			{
				if (SlashOverlap[i] != this && Cast<Achar_Unit>(SlashOverlap[i])->getUnitTeam() != this->getUnitTeam()) //Cast the overlapping actor and check its on the opposite team
				{
					Achar_Unit* hitTarg = Cast<Achar_Unit>(SlashOverlap[i]); //If it is then deal damage to that target
					//Damage is 180 + 75% bAD
					hitTarg->receiveDamage(180.f + (bonusAttack * 0.75)); // 180 + 75% bAD

				}
			}
			GetWorldTimerManager().SetTimer(Ability2Handle, Ability2CD, false); //Start the cooldown
		}

		

	}
}

void Achamp_Lucien::ability_3()
{
	UE_LOG(LogTemp, Warning, TEXT("Lucien Ability 3"));

	if (!GetWorldTimerManager().IsTimerActive(Ability3Handle)) //Not on cooldown
	{
		if (mana > 80.f)//Mana Cost
		{

			mana -= 80.f;
			TArray<AActor*> SlashOverlap;
			Slash->GetOverlappingActors(SlashOverlap, Achar_Unit::StaticClass()); //Get the actors
			UE_LOG(LogTemp, Warning, TEXT("Slash Overlap : %d"), SlashOverlap.Num());
			for (int i = 0; i < SlashOverlap.Num(); i++)
			{
				if (SlashOverlap[i] != this && Cast<Achar_Unit>(SlashOverlap[i])->getUnitTeam() != this->getUnitTeam()) //Cast the overlapping actor and check its on the opposite team
				{
					Achar_Unit* hitTarg = Cast<Achar_Unit>(SlashOverlap[i]); //If it is then deal damage to that target
					//Damage is 15 (Low) as main effect is the knockback
					hitTarg->receiveDamage(15.f); // 15
					//WIP add knockback

					FVector VecToTarg = ((hitTarg->GetActorLocation() - GetActorLocation())*FVector(1.f,1.f,0.f))+FVector(0.f,0.f,0.f);
					VecToTarg.Normalize();
					hitTarg->GetMovementComponent()->StopActiveMovement();
					VecToTarg = VecToTarg * 2000.f;
					hitTarg->LaunchCharacter(VecToTarg, false, false);
				}
			}
			GetWorldTimerManager().SetTimer(Ability3Handle, Ability3CD, false); //Start the cooldown
		}

		

	}
}

void Achamp_Lucien::BeginPlay()
{
	Super::BeginPlay();
}
