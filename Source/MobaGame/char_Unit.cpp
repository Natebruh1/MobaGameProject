// Fill out your copyright notice in the Description page of Project Settings.


#include "char_Unit.h"
#include "Moba_GameState.h"
#include "Moba_GameMode.h"

// Sets default values
Achar_Unit::Achar_Unit()//Constructor
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	bAlwaysRelevant = true;
	NetUpdateFrequency = 60;//High net update frequency since this is the player character

	//Add the unit targetability channel to the capsule
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Block);//Make Capsule block for targetability
	
	//Set the character stats
	unitTargetable = true;
	unitVisible = true;
	unitSize = 1.0f;
	unitName = FString(TEXT("Unit"));
	

	//set the projectile class
	GeneratedProjectileBP = Cast<UBlueprint>(StaticLoadObject(UObject::StaticClass(), NULL, *bpResource));


	
	//Setup the health bar
	healthBarWidgetComponent = CreateDefaultSubobject<UWidgetComponent>("Health Bar");
	healthBarWidgetComponent->SetWidgetClass(widgetClass);
	healthBarWidgetComponent->SetWidgetSpace(EWidgetSpace::World);
	healthBarWidgetComponent->SetDrawSize({ 3000.f,260.f });

	FRotator newRot = { 0.f,-90.f,0.f };
	FVector newMov = { 0.f,0.f,-80.f };
	GetMesh()->AddLocalRotation(newRot);
	GetMesh()->AddRelativeLocation(newMov);
	
}

// Called when the game starts or when spawned
void Achar_Unit::BeginPlay()
{
	Super::BeginPlay();
	SetReplicates(true);
	SetReplicateMovement(true);
	joinTeam(Blue);
	//Update our internal team
	setUnitTeam(Blue);
}

// Called every frame
void Achar_Unit::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (currentHealth <= 0.f)
	{
		Destroy();
	}
	currentHealth += healthRegen * DeltaTime;
}

// Called to bind functionality to input
void Achar_Unit::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void Achar_Unit::joinTeam(TeamName team)
{
	//First removes us from our current team

	
	if (GetWorld()->GetGameState<AMoba_GameState>()) {
		GetWorld()->GetGameState<AMoba_GameState>()->RemoveUnitFromTeam(this, this->getUnitTeam()); //(This automatically shortens the list as there can be no holes in a TArray
	}
	//Then add us to the new team
	if (!getTeamArray(team)->Contains(this)) //If we are not in the specified team we want to join (prevents data duplication)
	{
		UE_LOG(LogTemp, Warning, TEXT("Not in team we want to join... Adding us to the team array"));
		if (GetWorld()->GetGameState<AMoba_GameState>()) {
			GetWorld()->GetGameState<AMoba_GameState>()->AddUnitToTeam(this, team);
		}
		

	}
	TEnumAsByte<TeamName> byteTeam = team;
	setUnitTeam(team);
	
	
	
}


void Achar_Unit::joinOtherTeam() //Swaps team from red to blue and vice versa -- Useful for testing in the editor
{
	if (unitTeam == Blue)
	{
		joinTeam(Red);
		//setUnitTeam(Red);
	}
	else if (unitTeam == Red)
	{
		joinTeam(Blue);
		
		//setUnitTeam(Blue);
	}
}

TArray<Achar_Unit*>* Achar_Unit::getTeamArray(TEnumAsByte <TeamName> team)
{
	TArray<Achar_Unit*>* teamPointer= GetWorld()->GetGameState<AMoba_GameState>()->getTeamArray(team);
	return teamPointer;
	
}

void Achar_Unit::setUnitTargetable(bool val)
{
	unitTargetable = val;
}

void Achar_Unit::setUnitVisible(bool val)
{
	unitVisible = val;
}

void Achar_Unit::setUnitSize(float val)
{
	unitSize = val;
}

void Achar_Unit::setTargetUnit(Achar_Unit* val)
{
	targetedUnit = val;
	//*targetedUnitPtr = val;
}

void Achar_Unit::setUnitName(FString val)
{
	unitName = val;
}

void Achar_Unit::setUnitAttackRange(float val)
{
	unitAttackRange = val;
}

void Achar_Unit::setUnitRangeType(EUnitRangeType val)
{
	unitRangeType = val;
	
}

void Achar_Unit::setUnitTeam_Implementation(TeamName val)
{
	unitTeam = val;
}
bool Achar_Unit::setUnitTeam_Validate(TeamName val)
{
	return true;
}

bool Achar_Unit::getUnitTargetable()
{
	return unitTargetable;
}

bool Achar_Unit::getUnitVisible()
{
	return unitVisible;
}

float Achar_Unit::getUnitSize()
{
	return unitSize;
}

Achar_Unit* Achar_Unit::getTargetUnit()
{
	return targetedUnit;
}

FString Achar_Unit::getUnitName()
{
	return unitName;
}

TeamName Achar_Unit::getUnitTeam()
{
	return unitTeam;
}

float Achar_Unit::getUnitRange()
{
	return unitAttackRange;
}

void Achar_Unit::windupAttack()
{
	if (getUnitTeam() != getTargetUnit()->getUnitTeam()) //Check on same team
	{

		//Check if target is in range
		FVector targetVector = GetActorLocation() - (getTargetUnit()->GetActorLocation());
		if (targetVector.Length() < getUnitRange()) //If the vector between us and our target is less than our range then
		{
			GetWorld()->GetTimerManager().SetTimer(attackTimerHandle, this, &Achar_Unit::attackUnit, 1.f / attackSpeed, false); //Set a timer equal to our attacks per second to run attackUnit();
		}

	}
	
}

void Achar_Unit::attackUnit()
{
	UE_LOG(LogTemp, Warning, TEXT("Attacking a Unit"));
	FVector targetVector = GetActorLocation() - (getTargetUnit()->GetActorLocation());
	if (targetVector.Length() < getUnitRange()) //If still in range
	{
		//Spawn Champion In Level
		FVector SpawnLocation = FVector(GetActorLocation());//WIP set to team spawn points
		FRotator SpawnRotation = FRotator(0.f, 0.f, 0.f);
		Apn_attackProjectile* projRef = GetWorld()->SpawnActor<Apn_attackProjectile>(GeneratedProjectileBP->GeneratedClass, SpawnLocation, SpawnRotation);
		if (projRef)
		{
			if (getTargetUnit())
			{
				UE_LOG(LogTemp, Warning, TEXT("We have an available targeted unit"));
			}
			//Set projectile Target
			projRef->setTargetUnit(getTargetUnit());
			//Set projectile Damage
			projRef->storedDamage = baseAttack + bonusAttack;
			//Copy Projectile OnHits
			//projRef->OnHit = this->OnHit;
			projRef->OwnedUnit = this;
			//projRef->OnHit = this->OnHit;
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Spawned projectile is nullptr"));
		}

		attackProjectileArray.Add(projRef); //Spawn the attack projectile
	}
}

FTimerHandle* Achar_Unit::getAttackTimerHandle()
{
	return &attackTimerHandle;
}

void Achar_Unit::receiveDamage(float val)
{
	
	UE_LOG(LogTemp, Warning, TEXT("RECEIVING DAMAGE"));
	currentHealth -= val;
}

//void Achar_Unit::AddOnHit(FOnHitDelegate OnHitName)
//{
//	OnHit.Add(OnHitName);
//	
//}

void Achar_Unit::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps)const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(Achar_Unit, targetedUnit);
	DOREPLIFETIME(Achar_Unit, unitTeam);
	DOREPLIFETIME(Achar_Unit, currentHealth);
	DOREPLIFETIME(Achar_Unit, baseHealth);
	DOREPLIFETIME(Achar_Unit, healthRegen);
	
	//DOREPLIFETIME_CONDITION(APC_ChampController, cameraAttached, COND_OwnerOnly);
}