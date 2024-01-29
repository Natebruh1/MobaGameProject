// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <map>
#include <string>
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





template <typename... T>
using ScriptFunction = void (*)(T... args);
template <typename... T>
using ChampionScripts = std::map<std::string, ScriptFunction<T...>>;


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
	
	template <typename... T>
	ChampionScripts<T...>* get_scripts();

	
	virtual void ability_1();
	virtual void ability_1_Animation();
	//virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps)const override;
private:
	//Actually create the topdown Camera using a camera component
	UPROPERTY(VisibleAnywhere) class UCameraComponent* TopDownCameraComponent;

	//And do the same for the Camera Boom so that it sits above the player
	UPROPERTY(VisibleAnywhere) class USpringArmComponent* CameraBoom;

	void* scripts; //Encompasser that is only accessible via casting
	std::string chosenChampion = "Wao";
};

