// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "champ_WaoPierre.h"
#include "char_BaseChampion.h"
#include "AC_PlayerAIController.h"
#include "Templates/SubclassOf.h"
#include "Moba_GameState.h"

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"

#include "InputActionValue.h"

#include "PC_ChampController.generated.h"

//Forward declaration allows us to spawn Decal or FX upon clicking
class UNiagaraSystem;

/**
 * 
 */
UCLASS()
class MOBAGAME_API APC_ChampController : public APlayerController
{
	GENERATED_BODY()

public:
	//Constructor
	APC_ChampController();
	//The actual effect we want to spawn upon clicking
	UPROPERTY(EditAnywhere) UNiagaraSystem* CursorEffect;

	//Control Mapping to allow different actions based upon context of input
	UPROPERTY(EditAnywhere) class UInputMappingContext* DefaultMappingContext;

	//SetDestination from clicking
	UPROPERTY(EditAnywhere) class UInputAction* SetDestinationClickAction;

	//Set Target from clicking
	UPROPERTY(EditAnywhere) class UInputAction* SetTargetClickAction;

	//KEYS
	UPROPERTY(EditAnywhere) class UInputAction* UseAbility1;

	//Spawn champion
	
	UPROPERTY(EditAnywhere, Category = "Champion Class") TSubclassOf<AAC_PlayerAIController> aiClass;

	UPROPERTY(EditAnywhere, Category="Champion Class") TSubclassOf<Achar_BaseChampion> championClass;
	UPROPERTY(EditAnywhere, Replicated) Achar_BaseChampion* controlledChampion;


	
	UFUNCTION() void SpawnChampion();
	UFUNCTION(Client, Reliable, WithValidation) void ClientBindCameraToChampion();
	UFUNCTION(Server, Reliable, WithValidation) void ServerMoveChampion(FVector moveLocation);
	UFUNCTION(Server, Reliable, WithValidation) void ServerUpdateTargetUnit(Achar_Unit* newTarget);
	UFUNCTION(Server, Reliable, WithValidation) void Ability_1();
	UFUNCTION(NetMulticast, Reliable, WithValidation) void Ability_1_Animation();
	virtual void Tick(float DeltaTime) override;
protected:
	//Move to the mouse cursor
	uint32 bMoveToMouseCursor : 1;

	virtual void SetupInputComponent() override;
	virtual void BeginPlay();
	

	//Input Handler Functions
	void OnInputStarted();
	void OnSetDestinationTriggered();//Movement
	void OnSetDestinationReleased();
	void OnSetUnitTargetTriggered();//Targeting
	void OnSetUnitTargetReleased();

	UFUNCTION() void clientAbility1(); //Abilities

	UFUNCTION(Server, Reliable, WithValidation) void incrementPlayerCount();
	UFUNCTION(Server, Reliable, WithValidation)void setChampionTeam(TeamName val);
	UFUNCTION(CallInEditor, Server, Reliable, WithValidation) void joinOtherTeam();
private:
	FVector StoredDestination;
	float FollowTime;
	bool cameraAttached;
	FVector SpawnLocation;
	FRotator SpawnRotation;
	AAC_PlayerAIController* aiCont;
};
