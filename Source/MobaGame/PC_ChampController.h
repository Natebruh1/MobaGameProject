// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "champ_Lucien.h"
#include "champ_WaoPierre.h"
#include "./Public/W_SidePanelInfo.h"
#include "./Public/W_GameDiagnostics.h"
#include "./Public/playerCharState.h"
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
class UW_ChampSelect;
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
	UPROPERTY(EditAnywhere) class UInputAction* UseAbility2;
	UPROPERTY(EditAnywhere) class UInputAction* UseAbility3;

	//Spawn champion
	
	UPROPERTY(EditAnywhere, Category = "Champion Class") TSubclassOf<AAC_PlayerAIController> aiClass;

	UPROPERTY(EditAnywhere, Category="Champion Class") TSubclassOf<Achar_BaseChampion> championClass;
	UPROPERTY(EditAnywhere, Replicated) Achar_BaseChampion* controlledChampion;

	UPROPERTY(EditAnywhere) TSubclassOf<UW_SidePanelInfo> overlayClass;
	UPROPERTY(EditAnywhere) UW_SidePanelInfo* displayPtr;

	UPROPERTY(EditAnywhere) TSubclassOf<UW_GameDiagnostics> diagClass;
	UPROPERTY(EditAnywhere) UW_GameDiagnostics* diagPtr;

	UPROPERTY(EditAnywhere,Replicated) float cd1;
	UPROPERTY(EditAnywhere,Replicated) float cd2;
	UPROPERTY(EditAnywhere,Replicated) float cd3;
	
	UFUNCTION() void SpawnChampion();
	void CreatePlayer();
	UFUNCTION(Client, Reliable, WithValidation) void ClientBindCameraToChampion();
	UPROPERTY(EditAnywhere,Replicated)bool cameraAttached;

	UFUNCTION(Server, Reliable, WithValidation) void ServerMoveChampion(FVector moveLocation);
	UFUNCTION(Server, Reliable, WithValidation) void ServerUpdateTargetUnit(Achar_Unit* newTarget);
	UFUNCTION(Server, Reliable, WithValidation) void Ability_1();
	UFUNCTION(NetMulticast, Reliable, WithValidation) void Ability_1_Animation();
	UFUNCTION(Server, Reliable, WithValidation) void Ability_2();
	UFUNCTION(NetMulticast, Reliable, WithValidation) void Ability_2_Animation();
	UFUNCTION(Server, Reliable, WithValidation) void Ability_3();
	UFUNCTION(NetMulticast, Reliable, WithValidation) void Ability_3_Animation();
	virtual void Tick(float DeltaTime) override;

	UFUNCTION() FVector getMouseVec();
	UFUNCTION(Server, Reliable, WithValidation) void ServerUpdateMouseVec(FVector inp);

	UPROPERTY(EditAnywhere,Replicated) int playerID;
	UPROPERTY(EditAnywhere, Replicated) TEnumAsByte<EInternalActionState> stateFromServer;

	UPROPERTY(EditAnywhere, Replicated) FString netAction="";
	UFUNCTION(Client, Reliable, WithValidation)void ClientUpdateNetAction(const FString &inStr);


	UPROPERTY(EditAnywhere) TSubclassOf<UW_ChampSelect> champSelectClass;
	UPROPERTY(EditAnywhere) UW_ChampSelect* champSelectPtr;
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
	UFUNCTION() void clientAbility2();
	UFUNCTION() void clientAbility3();

	UFUNCTION(Server, Reliable, WithValidation) void incrementPlayerCount();
	UFUNCTION(Server, Reliable, WithValidation)void setChampionTeam(TeamName val);
	UFUNCTION(CallInEditor, Server, Reliable, WithValidation) void joinOtherTeam();


private:
	FVector StoredDestination;
	float FollowTime;
	
	FVector SpawnLocation;
	FRotator SpawnRotation;
	AAC_PlayerAIController* aiCont;
	
};
