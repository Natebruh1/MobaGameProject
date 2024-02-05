// Fill out your copyright notice in the Description page of Project Settings.


#include "PC_ChampController.h"

#include "Engine/World.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/Pawn.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "Moba_GameState.h"
#include "DrawDebugHelpers.h"



APC_ChampController::APC_ChampController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;
	StoredDestination = FVector::ZeroVector;
	FollowTime = 0.0f;
}


void APC_ChampController::SpawnChampion()
{
	//Spawn Champion In Level
	SpawnLocation = FVector(8050.0f, 18590.0f, 96.0f);//WIP set to team spawn points
	SpawnRotation = FRotator(0.f, 0.f, 0.f);
	//Spawn AI
	aiCont = GetWorld()->SpawnActor<AAC_PlayerAIController>(aiClass, SpawnLocation, SpawnRotation);

	
	UE_LOG(LogTemp, Warning, TEXT("Spawning Character"));
	
	bool b = (championClass == NULL);
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Black, FString::Printf(TEXT("Template Class Null: %s"), b ? TEXT("true") : TEXT("false")));
	controlledChampion = GetWorld()->SpawnActor<Achar_BaseChampion>(championClass, SpawnLocation, SpawnRotation);
	//controlledChampion->SetAutonomousProxy(true);
	
	aiCont->Possess(controlledChampion);
	
}


void APC_ChampController::Tick(float DeltaTime)
{
	if (!cameraAttached)
	{
		ClientBindCameraToChampion();
	}
}



void APC_ChampController::ClientBindCameraToChampion_Implementation()
{
	if (controlledChampion != nullptr)
	{
		//controlledChampion->SetOwner(this);
		//UE_LOG(LogTemp, Warning, TEXT("---Character reference found---"));
		cameraAttached = true;
		SetViewTargetWithBlend(controlledChampion, 0.f, EViewTargetBlendFunction::VTBlend_Linear);//We bind the camera
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Character reference not found"));
	}
	
}
bool APC_ChampController::ClientBindCameraToChampion_Validate() { return true; }



void APC_ChampController::SetupInputComponent()
{
	Super::SetupInputComponent();

	//Setup Action Bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent))
	{
		//Setup the inputs for the mouse
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Started, this, &APC_ChampController::OnInputStarted);
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Triggered, this, &APC_ChampController::OnSetDestinationTriggered);
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Completed, this, &APC_ChampController::OnSetDestinationReleased);
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Canceled, this, &APC_ChampController::OnSetDestinationReleased);

		//Left click (targeting)
		EnhancedInputComponent->BindAction(SetTargetClickAction, ETriggerEvent::Triggered, this, &APC_ChampController::OnSetUnitTargetTriggered);
		EnhancedInputComponent->BindAction(SetTargetClickAction, ETriggerEvent::Completed, this, &APC_ChampController::OnSetUnitTargetReleased);
		EnhancedInputComponent->BindAction(SetTargetClickAction, ETriggerEvent::Canceled, this, &APC_ChampController::OnSetUnitTargetReleased);

		//Setup the inputs for the keys
		EnhancedInputComponent->BindAction(UseAbility1, ETriggerEvent::Started, this, &APC_ChampController::clientAbility1);
		EnhancedInputComponent->BindAction(UseAbility2, ETriggerEvent::Started, this, &APC_ChampController::clientAbility2);
		//EnhancedInputComponent->BindAction(UseAbility1, ETriggerEvent::Completed, this, &APC_ChampController::clientAbility1);
		//EnhancedInputComponent->BindAction(UseAbility1, ETriggerEvent::Triggered, this, &APC_ChampController::clientAbility1);
	}
}

void APC_ChampController::BeginPlay()
{
	Super::BeginPlay();

	//Add the mapping Context
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(DefaultMappingContext, 0);
	}
	//Spawn the champion
	if (HasAuthority())//We only want the player controller that is on the server to spawn a champion
	{
		UE_LOG(LogTemp, Warning, TEXT("SPAWNING CHAMP!"));
		SpawnChampion();
		incrementPlayerCount();
	}
	cameraAttached = false;
	ClientBindCameraToChampion();
	
	if (GetWorld()->GetGameState<AMoba_GameState>()->getPlayerCount() % 2 == 0) {
		joinOtherTeam();
	}
	
}

void APC_ChampController::OnInputStarted()
{	
	//Cancel the previous input
	StopMovement();
	
	
}

void APC_ChampController::OnSetDestinationTriggered()
{
	//Flag that we've pressed the input
	FollowTime += GetWorld()->GetDeltaSeconds();

	//Line trace to find where the mouse has been clicked
	FHitResult Hit;
	bool bHitSuccessful = false;
	bool bTargetSuccessful = false;
	//Were they targeting a character? If yes then move until in targets range
	bTargetSuccessful = GetHitResultUnderCursor(ECollisionChannel::ECC_GameTraceChannel1, true, Hit);
	if (Hit.GetActor())//If we hit an actor
	{
		UE_LOG(LogTemp, Warning, TEXT("potential target found"));
		if (controlledChampion->getUnitTeam()!= Cast<Achar_Unit>(Hit.GetActor())->getUnitTeam())//If the actor we are trying to target is not on the same team as the
		{
			UE_LOG(LogTemp, Warning, TEXT("On opposite teams"));
			ServerUpdateTargetUnit(Cast<Achar_Unit>(Hit.GetActor()));//Run the set target function in the player controller to set our target as our enemy
			//WIP call server function to aicontroller to update state to AttackMove
		}
		
	}


	if (!bTargetSuccessful)//If we didnt try to attack an enemy
	{

		bHitSuccessful = GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, true, Hit);//WIP update for targeting
		UE_LOG(LogTemp, Warning, TEXT("Attempted Hit"));
		
		if (bHitSuccessful)//If we click on a travellable surface
		{
			UE_LOG(LogTemp, Warning, TEXT("Succesful Hit : %s"), *Hit.Location.ToString());
			StoredDestination = Hit.Location;
			//Move the character we're controlling towards
			//Call the AI Controller of our targeted player
			//UE_LOG(LogTemp, Warning, TEXT("Hit Location : %s"), *StoredDestination.ToString());
			ServerMoveChampion(StoredDestination);


		}
	}
	
	
	
}



void APC_ChampController::OnSetDestinationReleased()
{
}

void APC_ChampController::OnSetUnitTargetTriggered()
{
	//Line trace to find where the mouse has been clicked
	FHitResult Hit;
	bool bHitSuccessful = false;


	bHitSuccessful = GetHitResultUnderCursor(ECollisionChannel::ECC_GameTraceChannel1, true, Hit);//WIP update for targeting
	if (bHitSuccessful)
	{
		//Update our targeted unit to the one we just clicked
		if (Hit.GetActor())
		{
			
			ServerUpdateTargetUnit(Cast<Achar_Unit>(Hit.GetActor()));//Run the set target function in the player controller
			
		}
		
	}
	else {
		//Remove the currently targeted unit
		ServerUpdateTargetUnit(nullptr);
	}
	
}





void APC_ChampController::ServerUpdateTargetUnit_Implementation(Achar_Unit* newTarget)
{
	if (controlledChampion->GetController<AAC_PlayerAIController>() != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("player ai controller found, targeting"))
		controlledChampion->GetController<AAC_PlayerAIController>()->ServerSetTargetUnit(newTarget);

		if (newTarget != nullptr) {
			if (controlledChampion->getUnitTeam() != newTarget->getUnitTeam()) //If new target is on the other team
			{
				controlledChampion->GetController<AAC_PlayerAIController>()->updateInternalState(AttackMove);
			}
		}
		
	}
}
bool APC_ChampController::ServerUpdateTargetUnit_Validate(Achar_Unit* newTarget) { return true; }

void APC_ChampController::OnSetUnitTargetReleased()
{
}

void APC_ChampController::clientAbility1()
{
	UE_LOG(LogTemp, Warning, TEXT("Player Controller calling Ability 1 from client"));
	Ability_1();
	Ability_1_Animation();
}

void APC_ChampController::clientAbility2()
{
	UE_LOG(LogTemp, Warning, TEXT("Player Controller calling Ability 2 from client"));
	Ability_2();
	Ability_2_Animation();
}

/// ---------///
/// ABILITIES///
/// ---------///
void APC_ChampController::Ability_1_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("Player Controller calling Ability 1"));
	
	controlledChampion->ability_1();
		
	
	
	
}

bool APC_ChampController::Ability_1_Validate() { return true; }


void APC_ChampController::Ability_1_Animation_Implementation()
{
	controlledChampion->ability_1_Animation();
}
bool APC_ChampController::Ability_1_Animation_Validate() { return true; }


void APC_ChampController::Ability_2_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("Player Controller calling Ability 2"));

	controlledChampion->ability_2();
}
bool APC_ChampController::Ability_2_Validate() { return true; }

void APC_ChampController::Ability_2_Animation_Implementation()
{
	controlledChampion->ability_2_Animation();
}
bool APC_ChampController::Ability_2_Animation_Validate() { return true; }

void APC_ChampController::incrementPlayerCount_Implementation()
{
	AMoba_GameState* gameState = GetWorld()->GetGameState<AMoba_GameState>();
	gameState->setPlayerCount(gameState->getPlayerCount() + 1);
	
}
bool APC_ChampController::incrementPlayerCount_Validate() { return true; }

void APC_ChampController::joinOtherTeam_Implementation()//WIPWIP
{

	if (controlledChampion->GetController<AAC_PlayerAIController>() != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("player ai controller found, swapping team"))
		controlledChampion->GetController<AAC_PlayerAIController>()->swapTeams();


	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("player ai controller not found"))
	}
}
bool APC_ChampController::joinOtherTeam_Validate()
{
	return true;
}

void APC_ChampController::setChampionTeam_Implementation(TeamName val)
{
	controlledChampion->setUnitTeam(val);
}

bool APC_ChampController::setChampionTeam_Validate(TeamName val)
{
	return true;
}

void APC_ChampController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps)const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(APC_ChampController, controlledChampion);
	//DOREPLIFETIME_CONDITION(APC_ChampController, cameraAttached, COND_OwnerOnly);
}




void APC_ChampController::ServerMoveChampion_Implementation(FVector moveLocation)
{
	if (controlledChampion->GetController<AAC_PlayerAIController>() != nullptr) //If we can get a reference to the AI controller and cast it as a  playerAI controller
	{
		UE_LOG(LogTemp, Warning, TEXT("AI Controller Found"));
		UE_LOG(LogTemp, Warning, TEXT("Moving to : %s"), *moveLocation.ToString());
		controlledChampion->GetController<AAC_PlayerAIController>()->setCachedMoveLocation(moveLocation);
		controlledChampion->GetController<AAC_PlayerAIController>()->updateInternalState(MoveState);
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("AI Controller not Found"));
		if (controlledChampion == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("Controlled champion not Found either"));
		}
	}
}
bool APC_ChampController::ServerMoveChampion_Validate(FVector moveLocation) { return true; }