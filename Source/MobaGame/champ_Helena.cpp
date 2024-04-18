// Fill out your copyright notice in the Description page of Project Settings.


#include "champ_Helena.h"
#include "AC_PlayerAIController.h"

Achamp_Helena::Achamp_Helena()
{
	chosenChampion = FString("Helena");
	//Set base stats
	Ability1CD = 5.f;
	Ability3CD = 18.f; //Long Cooldown to prevent spam healing
	Ability3CD = 0.f; //Toggleable
	setUnitAttackRange(900.f);
	baseAttack = 120.f;
	startHealth = 1200.f; //Lower health than 1500 as this is a mage
	currentHealth = 1200.f;


	secondaryName = FText::FromString("Mana");
	Ability1Desc = FText::FromString("You have no Auto Attacks. Shoot a energy ball (4 Charges)");
	Ability2Desc = FText::FromString("Heal yourself and nearby allies");
	Ability3Desc = FText::FromString("Float towards your mouse, starts quick but slows. Toggleable");

	ChampionScripts<Achamp_Helena, AAC_PlayerAIController*> tmpScripts;
	


	HealAura = CreateDefaultSubobject<USphereComponent>(TEXT("Heal Sphere")); //For overlapping
	HealAura->SetupAttachment(GetRootComponent());
	HealAura->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Overlap);
	HealAura->SetSphereRadius(160.f * baseSize);
	//tmpScripts.insert({ std::string("Dash"),&Achamp_Helena::ScriptDashDirection });
	//scripts = &tmpScripts;
}

void Achamp_Helena::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	secondaryPercentage = mana / maxMana;


	if (Ability3Active)
	{
		SetActorRotation(lockedInRot);
		//Cost 40 Mana per second
		

		if (GetController())
		{
			GetController<AAC_PlayerAIController>()->MoveControlledPlayer(GetActorLocation() + lockedInFloatPos * 100.f);
			GetController<AAC_PlayerAIController>()->setCachedMoveLocation(GetActorLocation() + lockedInFloatPos * 100.f);
			
			GetCharacterMovement()->MaxWalkSpeed = dashSpeed;
			dashSpeed -= (oldMoveSpeed / 1.2f) * DeltaTime;
			dashSpeed = max(dashSpeed, 100.f);
		}
		if (mana < 40 * DeltaTime)
		{
			Ability3Active = false; //Disable this ability
			GetMovementComponent()->StopActiveMovement(); //Stop Moving
			GetController<AAC_PlayerAIController>()->bAllowStrafe = false;
			GetController<AAC_PlayerAIController>()->setCachedMoveLocation(GetActorLocation());
			GetCharacterMovement()->MaxWalkSpeed = 600.f;
		}
		else
		{
			//Cost the mana
			mana -= 40 * DeltaTime;
		}

	}
	else
	{
		dashSpeed += (oldMoveSpeed / 1.2f) * DeltaTime;
		dashSpeed = min(dashSpeed, oldMoveSpeed*1.7f);
	}
}

void Achamp_Helena::ability_1()
{
	if (!GetWorldTimerManager().IsTimerActive(Ability1Handle)) //Not on cooldown
	{
		Apn_baseAimedProjectile* newProj = GetWorld()->SpawnActor<Apn_baseAimedProjectile>(AbilityProjectile, GetActorLocation(), mouseVec.Rotation());
		newProj->team = getUnitTeam();
		newProj->baseDir = mouseVec * FVector(1.f, 1.f, 0.f);
		GetWorldTimerManager().SetTimer(Ability1Handle, Ability1CD, false); //Start the cooldown
	}
}

void Achamp_Helena::ability_2()
{
	if (!GetWorldTimerManager().IsTimerActive(Ability2Handle)) //Not on cooldown
	{
		TArray<AActor*> SlashOverlap;
		HealAura->GetOverlappingActors(SlashOverlap, Achar_Unit::StaticClass()); //Get the actors
		UE_LOG(LogTemp, Warning, TEXT("Slash Overlap : %d"), SlashOverlap.Num());
		for (int i = 0; i < SlashOverlap.Num(); i++)
		{
			if (SlashOverlap[i] != this && Cast<Achar_Unit>(SlashOverlap[i])->getUnitTeam() == this->getUnitTeam()) //Cast the overlapping actor and check its on the opposite team
			{
				Achar_Unit* hitTarg = Cast<Achar_Unit>(SlashOverlap[i]); //If it is then heal the target
				
				hitTarg->receiveDamage(-120.f); 

			}
		}

		GetWorldTimerManager().SetTimer(Ability2Handle, Ability2CD, false); //Start the cooldown
	}
}

void Achamp_Helena::ability_3()
{

	if (!Ability3Active)//Enable Ability
	{
		GetController<AAC_PlayerAIController>()->bAllowStrafe = true;
		oldMoveSpeed = 600.f;
		

	}
	else//Disable Ability
	{
		GetMovementComponent()->StopActiveMovement();//Stop Movement
		GetCharacterMovement()->MaxWalkSpeed = 600.f;
		GetController<AAC_PlayerAIController>()->bAllowStrafe = false;
		GetController<AAC_PlayerAIController>()->setCachedMoveLocation(GetActorLocation());
	}
	Ability3Active = !Ability3Active; //Toggle Ability3
	lockedInFloatPos = mouseVec;
	lockedInFloatPos.Normalize();
	lockedInRot = GetActorRotation();
}

void Achamp_Helena::attackUnit()
{
	//Do Nothing
}

void Achamp_Helena::BeginPlay()
{
	Super::BeginPlay();
}



void Achamp_Helena::ScriptDashDirection(AAC_PlayerAIController* args)
{
	args->MoveControlledPlayer(lockedInFloatPos);
}