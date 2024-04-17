// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include <minmax.h>
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Materials/Material.h"
#include "Engine/World.h"
#include "Templates/SubclassOf.h"
#include "Net/UnrealNetwork.h"
#include "pn_attackProjectile.h"
#include "Components/WidgetComponent.h"
//#include "Moba_GameState.h" //Include to get team Enum

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "char_Unit.generated.h"

DECLARE_DELEGATE_TwoParams(FOnHitDelegate,Achar_Unit*,Achar_Unit**)



UENUM() enum TeamName
{
	Red UMETA(DisplayName = "Red Team"),
	Blue UMETA(DisplayName = "Blue Team"),
	Neutral UMETA(DisplayName = "Neutral Team"),
};

UENUM() enum EUnitRangeType
{
	Melee UMETA(DisplayName = "Melee"),
	Ranged UMETA(DisplayName = "Ranged"),
};

//Forward Declarations
//class AMoba_GameState;

UCLASS()
class MOBAGAME_API Achar_Unit : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	Achar_Unit();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FString chosenChampion = "Unit";
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//Join a team
	void joinTeam(TeamName team); //Callable in editor so we can test with champions on different teams
	void joinOtherTeam();
	//Get a team array from team enum
	TArray<Achar_Unit*>* getTeamArray(TEnumAsByte <TeamName> team); //Returns a pointer to the TArray, which is filled with pointers to units.


	
	//Setters and getters (non-virtual but available to all units)
	//Setters
	void setUnitTargetable(bool val);
	void setUnitVisible(bool val);
	void setUnitSize(float val);
	void setTargetUnit(Achar_Unit* val);
	void setUnitName(FString val);
	void setUnitAttackRange(float val);
	void setUnitRangeType(EUnitRangeType val);
	UFUNCTION(Server, Reliable, WithValidation)void setUnitTeam(TeamName val);

	//Getters
	bool getUnitTargetable();
	bool getUnitVisible();
	float getUnitSize();
	Achar_Unit* getTargetUnit();
	FString getUnitName();
	TeamName getUnitTeam();
	float getUnitRange();

	void windupAttack();
	virtual void attackUnit();
	FTimerHandle* getAttackTimerHandle();
	virtual void receiveDamage(float val);

	
	FString bpResource = "/Game/BP_pn_attackProjectile.BP_pn_attackProjectile";
	UBlueprint* GeneratedProjectileBP;
	
	//Health variables
	UPROPERTY(EditAnywhere, Category = "Health Variables", Replicated) float bonusHealth = 0.f;
	UPROPERTY(EditAnywhere, Category = "Health Variables", Replicated) float startHealth = 1500.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health Variables", Replicated) float baseHealth = 1500.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health Variables", Replicated) float currentHealth = baseHealth-300.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health Variables", Replicated) float healthRegen = 3.f;
	UPROPERTY(EditAnywhere) float baseAttack = 140.f;
	UPROPERTY(EditAnywhere) float bonusAttack = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health Variables") TSubclassOf<UUserWidget> widgetClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health Variables") UWidgetComponent* healthBarWidgetComponent;

	float baseSize = 1.f;
	TArray<FOnHitDelegate> OnHit;
	//UFUNCTION() void AddOnHit(FOnHitDelegate OnHitName);
	Achar_Unit** targetedUnitPtr;

	
private:
	//Unit stats
	UPROPERTY(EditAnywhere) bool unitTargetable;
	UPROPERTY(EditAnywhere) bool unitVisible;
	UPROPERTY(EditAnywhere) float unitSize;
	UPROPERTY(EditAnywhere, Replicated) Achar_Unit* targetedUnit;
	

	UPROPERTY(EditAnywhere) FString unitName;
	UPROPERTY(EditAnywhere, Replicated) TEnumAsByte<TeamName> unitTeam; //Team that the unit is a part of
	UPROPERTY(EditAnywhere) float unitAttackRange = 512.f; //About 4% of the map
	UPROPERTY(EditAnywhere) TEnumAsByte<EUnitRangeType> unitRangeType=Ranged;
	UPROPERTY(EditAnywhere) float attackSpeed = 0.625f;


	
	

	FTimerHandle attackTimerHandle;
	TArray<Apn_attackProjectile*> attackProjectileArray;
	
};
