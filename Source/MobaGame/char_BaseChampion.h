// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <map>
#include <string>
#include "Kismet/GameplayStatics.h"
#include "ChampionIncludes.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/DecalComponent.h"
#include "ChampionIncludes.h"


#include "CoreMinimal.h"
#include "char_Unit.h"
#include "char_BaseChampion.generated.h"





template <typename c, typename... T>
using ScriptFunction = void (c::*)(T... args);
template <typename c,typename... T>
using ChampionScripts = std::map<std::string, ScriptFunction<c,T...>>;


/**
 * 
 */
UCLASS()
class MOBAGAME_API Achar_BaseChampion : public Achar_Unit
{
	GENERATED_BODY()
	


public:

	virtual void Tick(float DeltaTime) override;
	Achar_BaseChampion();

	//Return the Camera Component
	FORCEINLINE class UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }
	//Return the Camera Boom/ Spring arm object
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	//ForceInline means each time the function is called the function body is inserted, this is an unreal macro but there are vs macros for it too.
	
	template <typename c,typename... T>
	ChampionScripts<c,T...>* get_scripts();

	UPROPERTY(EditAnywhere)FVector mouseVec;
	
	UFUNCTION() virtual void ability_1();
	UFUNCTION() virtual void ability_1_Animation();
	UFUNCTION() virtual void ability_2();
	UFUNCTION() virtual void ability_2_Animation();
	UFUNCTION() virtual void ability_3();
	UFUNCTION() virtual void ability_3_Animation();

	
	//virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps)const override;

	float Ability1CD = 10.f;
	UPROPERTY(EditAnywhere, Replicated)FTimerHandle Ability1Handle;
	FText Ability1Desc = FText::FromString("Q Desc");
	float Ability2CD = 10.f;
	UPROPERTY(EditAnywhere, Replicated)FTimerHandle Ability2Handle;
	FText Ability2Desc = FText::FromString("W Desc");
	float Ability3CD = 10.f;
	UPROPERTY(EditAnywhere, Replicated)FTimerHandle Ability3Handle;
	FText Ability3Desc = FText::FromString("E Desc");


	UPROPERTY(EditAnywhere, Replicated)float secondaryPercentage = 0.f;
	FText secondaryName = FText::FromString("Mana");

	UPROPERTY(EditAnywhere,Replicated)float mana = 0.f;
	float maxMana = 1000.f;
	float manaRegen = 10.f;

	void* scripts; //Encompasser that is only accessible via casting

private:
	//Actually create the topdown Camera using a camera component
	UPROPERTY(VisibleAnywhere) class UCameraComponent* TopDownCameraComponent;

	//And do the same for the Camera Boom so that it sits above the player
	UPROPERTY(VisibleAnywhere) class USpringArmComponent* CameraBoom;

	
};

