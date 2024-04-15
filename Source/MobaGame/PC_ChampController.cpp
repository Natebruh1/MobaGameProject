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
#include "./Public/W_ChampSelect.h"



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
	bool b;
	switch (currentChampion % totalChampions)
	{
	case 0:
		b = (championClass0 == NULL);
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Black, FString::Printf(TEXT("Template Class Null: %s"), b ? TEXT("true") : TEXT("false")));
		controlledChampion = GetWorld()->SpawnActor<Achar_BaseChampion>(championClass0, SpawnLocation, SpawnRotation);
		break;
	case 1:
		b = (championClass1 == NULL);
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Black, FString::Printf(TEXT("Template Class Null: %s"), b ? TEXT("true") : TEXT("false")));
		controlledChampion = GetWorld()->SpawnActor<Achar_BaseChampion>(championClass1, SpawnLocation, SpawnRotation);
		break;
	}

	
	//controlledChampion->SetAutonomousProxy(true);
	
	aiCont->Possess(controlledChampion);
	
	
}

void APC_ChampController::CreatePlayer(bool firstTime)
{
	if (HasAuthority())//We only want the player controller that is on the server to spawn a champion
	{
		UE_LOG(LogTemp, Warning, TEXT("SPAWNING CHAMP!"));
		SpawnChampion();
		if (firstTime) {// If we're spawning for the first time
			incrementPlayerCount();
			playerID = GetWorld()->GetGameState<AMoba_GameState>()->getPlayerCount();
		}
		
	}
	cameraAttached = false;
	ClientBindCameraToChampion();

	if (firstTime) {
		if (GetWorld()->GetGameState<AMoba_GameState>()->getPlayerCount() % 2 == 0) {
			joinOtherTeam();
		}
	}
	if (controlledChampion)
	{
		if (overlayClass) //Overlay UClass Ref
		{
			displayPtr = CreateWidget<UW_SidePanelInfo>(GetWorld(), overlayClass);
			if (displayPtr)
			{
				//displayPtr->SetDesiredSizeInViewport(FVector2D(500.f, 500.f));


				//cardPtr->SetRenderScale(FVector2D(0.1f, 0.1f));
				displayPtr->SetPositionInViewport(FVector2D(40.f, 190.f));


				displayPtr->SetRenderScale(FVector2D(0.5f, 0.5f));
				displayPtr->AddToViewport();
			}
		}
	}
	if (diagClass) //Diagnostic UClass ref
	{
		diagPtr = CreateWidget<UW_GameDiagnostics>(GetWorld(), diagClass);
		if (diagPtr) //Successfully made widget
		{

			if (GEngine->GameViewport) {
				FVector2D winSize = FVector2D(GEngine->GameViewport->Viewport->GetSizeXY());
				winSize -= FVector2D(winSize.X * 0.15, winSize.Y * 0.75);
				diagPtr->SetPositionInViewport(winSize / 2);
			}
			//diagPtr->SetRenderScale(FVector2D(5.f, 5.f));

			diagPtr->AddToViewport();
		}
	}
}

void APC_ChampController::DeletePlayer()
{
	if (HasAuthority())//We only want the player controller that is on the server to delete a champion
	{
		UE_LOG(LogTemp, Warning, TEXT("DELETING CHAMP!"));
		//aiCont->UnPossess();
		controlledChampion->Destroy();
		
		
		
	}
	//controlledChampion = nullptr;
	if (displayPtr)
	{
		displayPtr->RemoveFromViewport();
	}
	if (diagPtr)
	{
		diagPtr->RemoveFromViewport();
	}
	//
}


void APC_ChampController::switchChampion_Implementation()
{
	currentChampion += 1;
	DeletePlayer(); //Respawn the character
	CreatePlayer(false);
}
bool APC_ChampController::switchChampion_Validate() { return true; }
void APC_ChampController::respawnPlayer_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("Respawning Player"));
	if (!controlledChampion)
	{
		CreatePlayer(false);
	}
	
}
bool APC_ChampController::respawnPlayer_Validate() { return true; }
void APC_ChampController::Tick(float DeltaTime)
{
	if (!cameraAttached)
	{
		ClientBindCameraToChampion();
	}
	
	if (controlledChampion && IsLocalController())
	{
		//Display for Game
		if (displayPtr)
		{
			
			Cast<UW_SidePanelInfo>(displayPtr)->healthPercentage = controlledChampion->currentHealth / controlledChampion->baseHealth;
			
			Cast<UW_SidePanelInfo>(displayPtr)->secondaryResourcePercentage = controlledChampion->secondaryPercentage;
			Cast<UW_SidePanelInfo>(displayPtr)->secondaryName = controlledChampion->secondaryName;
			Cast<UW_SidePanelInfo>(displayPtr)->QDesc = controlledChampion->Ability1Desc;
			Cast<UW_SidePanelInfo>(displayPtr)->WDesc = controlledChampion->Ability2Desc;
			Cast<UW_SidePanelInfo>(displayPtr)->EDesc = controlledChampion->Ability3Desc;
			Cast<UW_SidePanelInfo>(displayPtr)->qCooldown = FText::FromString(FString::SanitizeFloat(floor(cd1)));
			Cast<UW_SidePanelInfo>(displayPtr)->wCooldown = FText::FromString(FString::SanitizeFloat(floor(cd2)));
			Cast<UW_SidePanelInfo>(displayPtr)->eCooldown = FText::FromString(FString::SanitizeFloat(floor(cd3)));//Cooldown
			
		}
		//Display for diagnostics
		if (diagPtr)
		{


			Cast<UW_GameDiagnostics>(diagPtr)->ActorName = FText::FromString(controlledChampion->GetActorLabel());
			Cast<UW_GameDiagnostics>(diagPtr)->playerID = playerID;
			Cast<UW_GameDiagnostics>(diagPtr)->fps = 1.f / DeltaTime;

			Cast<UW_GameDiagnostics>(diagPtr)->cachedMoveLocation = StoredDestination;
			Cast<UW_GameDiagnostics>(diagPtr)->currentState = stateFromServer;
			Cast<UW_GameDiagnostics>(diagPtr)->targetVector = getMouseVec();
			Cast<UW_GameDiagnostics>(diagPtr)->totalAttack = controlledChampion->baseAttack + controlledChampion->bonusAttack;//Attack
			Cast<UW_GameDiagnostics>(diagPtr)->currentHealth = (controlledChampion->currentHealth);//Health
			Cast<UW_GameDiagnostics>(diagPtr)->unitSize = (controlledChampion->getUnitSize());//Size
			Cast<UW_GameDiagnostics>(diagPtr)->secondaryResourcePercentage = (controlledChampion->secondaryPercentage);//Secondary Resource (Mana, Rage, Stagger ETC)
			Cast<UW_GameDiagnostics>(diagPtr)->targetedUnit = (controlledChampion->getTargetUnit());//Targeted unit

			Cast<UW_GameDiagnostics>(diagPtr)->CurrentNetworkAction = FText::FromString(netAction);//net Action
		}
	}
	if (controlledChampion && !IsLocalController() && displayPtr)
	{
		
		
		cd1 = controlledChampion->Ability1CD - GetWorld()->GetTimerManager().GetTimerElapsed(controlledChampion->Ability1Handle);
		
		cd2= controlledChampion->Ability2CD -GetWorld()->GetTimerManager().GetTimerElapsed(controlledChampion->Ability2Handle);
		
		cd3 = controlledChampion->Ability2CD -GetWorld()->GetTimerManager().GetTimerElapsed(controlledChampion->Ability3Handle);
		
	}
	if (controlledChampion && !IsLocalController() && diagPtr && controlledChampion->GetController<AAC_PlayerAIController>())
	{


		stateFromServer= controlledChampion->GetController<AAC_PlayerAIController>()->getCurrentState();

	}

	

	if (controlledChampion)//Update the vector to mouse
	{
		FVector tmp = getMouseVec();
		if (tmp != controlledChampion->GetActorLocation())
		{
			ServerUpdateMouseVec(tmp);
		}
		
	}


	if (!controlledChampion && GetWorld()->GetTimerManager().GetTimerElapsed(respawnTimer) == 0.f) //Dead, start the respawn timer
	{
		UE_LOG(LogTemp, Warning, TEXT("Player Presumed dead, starting respawn timer"));
		
		
		GetWorld()->GetTimerManager().SetTimer(respawnTimer, this, &APC_ChampController::respawnPlayer, 10.f, false);
		
		
	}
}

FVector APC_ChampController::getMouseVec()
{
	FHitResult Hit;
	bool bHitSuccessful = false;
	bool bTargetSuccessful = false;
	//Were they targeting a character? If yes then move until in targets range

	bHitSuccessful = GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, true, Hit);
	if (bHitSuccessful)//If we click on a travellable surface
	{
		//UE_LOG(LogTemp, Warning, TEXT("Found vec to Mouse : %s"), *(Hit.Location - controlledChampion-> GetActorLocation()).ToString());
		return Hit.Location - controlledChampion->GetActorLocation();


	}
	//UE_LOG(LogTemp, Warning, TEXT("Mouse Vector not found"));
	return controlledChampion->GetActorLocation();
}

void APC_ChampController::ServerUpdateMouseVec_Implementation(FVector inp)
{
	if (controlledChampion->GetController<AAC_PlayerAIController>() != nullptr)
	{
		//UE_LOG(LogTemp, Warning, TEXT("player ai controller Updating Cached MouseVec"));
		controlledChampion->GetController<AAC_PlayerAIController>()->cachedMouseVec=inp;
	}
	//ClientUpdateNetAction("APC_ChampController::ServerUpdateMouseVec_Implementation");
}
bool APC_ChampController::ServerUpdateMouseVec_Validate(FVector inp) { return true; };


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



void APC_ChampController::ClientUpdateNetAction_Implementation(const FString &inStr)
{
	netAction = inStr;
}
bool APC_ChampController::ClientUpdateNetAction_Validate(const FString &inStr) { return true; }
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
		EnhancedInputComponent->BindAction(UseAbility3, ETriggerEvent::Started, this, &APC_ChampController::clientAbility3);

		//Champion Switch
		EnhancedInputComponent->BindAction(SwitchChampion, ETriggerEvent::Started, this, &APC_ChampController::switchChampion);
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
	CreatePlayer();
	
}

void APC_ChampController::OnInputStarted()
{	
	//Cancel the previous input
	StopMovement();
	
	
}

/// ------- ///
/// CONTROL ///
/// ------- ///

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
				controlledChampion->GetController<AAC_PlayerAIController>()->updateInternalState(AttackMove); //Start AttackMoving towards them
			}
		}
		
	}
	
	ClientUpdateNetAction("APC_ChampController::ServerUpdateTargetUnit_Implementation");
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
void APC_ChampController::clientAbility3()
{
	UE_LOG(LogTemp, Warning, TEXT("Player Controller calling Ability 3 from client"));
	Ability_3();
	Ability_3_Animation();
}

/// ---------///
/// ABILITIES///
/// ---------///
void APC_ChampController::Ability_1_Implementation()
{
	//Cast Ability1
	UE_LOG(LogTemp, Warning, TEXT("Player Controller calling Ability 1"));
	
	controlledChampion->ability_1();
		
	
	
	
	ClientUpdateNetAction("APC_ChampController::Ability_1_Implementation");
}

bool APC_ChampController::Ability_1_Validate() { return true; }


void APC_ChampController::Ability_1_Animation_Implementation()
{
	controlledChampion->ability_1_Animation();
}
bool APC_ChampController::Ability_1_Animation_Validate() { return true; }


void APC_ChampController::Ability_2_Implementation()
{
	//Cast Ability2
	UE_LOG(LogTemp, Warning, TEXT("Player Controller calling Ability 2"));

	controlledChampion->ability_2();
	
	ClientUpdateNetAction("APC_ChampController::Ability_2_Implementation");
}
bool APC_ChampController::Ability_2_Validate() { return true; }

void APC_ChampController::Ability_2_Animation_Implementation()
{
	controlledChampion->ability_2_Animation();
}
bool APC_ChampController::Ability_2_Animation_Validate() { return true; }


void APC_ChampController::Ability_3_Implementation()
{
	//Cast Ability3
	UE_LOG(LogTemp, Warning, TEXT("Player Controller calling Ability 3"));

	controlledChampion->ability_3();
	
	ClientUpdateNetAction("APC_ChampController::Ability_3_Implementation");
}
bool APC_ChampController::Ability_3_Validate() { return true; }

void APC_ChampController::Ability_3_Animation_Implementation()
{
	controlledChampion->ability_3_Animation();
}
bool APC_ChampController::Ability_3_Animation_Validate() { return true; }

/// --------- ///
/// GAMESTATE ///
/// --------- ///

void APC_ChampController::incrementPlayerCount_Implementation()
{
	//Increment totalPlayers in the game
	AMoba_GameState* gameState = GetWorld()->GetGameState<AMoba_GameState>();
	gameState->setPlayerCount(gameState->getPlayerCount() + 1);
	
	ClientUpdateNetAction("APC_ChampController::incrementPlayerCount_Implementation");
}
bool APC_ChampController::incrementPlayerCount_Validate() { return true; }

void APC_ChampController::joinOtherTeam_Implementation()
{
	//Swap the team on the server of controlledChampion
	if (controlledChampion && controlledChampion->GetController<AAC_PlayerAIController>() != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("player ai controller found, swapping team"))
		controlledChampion->GetController<AAC_PlayerAIController>()->swapTeams();


	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("player ai controller not found"))
	}
	
	ClientUpdateNetAction("APC_ChampController::joinOtherTeam_Implementation");
}
bool APC_ChampController::joinOtherTeam_Validate()
{
	return true;
}

void APC_ChampController::setChampionTeam_Implementation(TeamName val)
{
	//Set the champion team on the server
	controlledChampion->setUnitTeam(val);
	
	ClientUpdateNetAction("APC_ChampController::setChampionTeam_Implementation");
}

bool APC_ChampController::setChampionTeam_Validate(TeamName val)
{
	return true;
}

void APC_ChampController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps)const
{
	//Replicated Properties for playerController (and some for champion)
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(APC_ChampController, controlledChampion);
	DOREPLIFETIME(APC_ChampController, stateFromServer);
	DOREPLIFETIME(APC_ChampController, playerID);

	DOREPLIFETIME(APC_ChampController, cd1);
	DOREPLIFETIME(APC_ChampController, cd2);
	DOREPLIFETIME(APC_ChampController, cd3);

	DOREPLIFETIME(APC_ChampController, cameraAttached);
	DOREPLIFETIME(APC_ChampController, netAction);
	//DOREPLIFETIME_CONDITION(APC_ChampController, cameraAttached, COND_OwnerOnly);
}




void APC_ChampController::ServerMoveChampion_Implementation(FVector moveLocation)
{
	if (controlledChampion->GetController<AAC_PlayerAIController>() != nullptr) //If we can get a reference to the AI controller and cast it as a  playerAI controller
	{
		UE_LOG(LogTemp, Warning, TEXT("AI Controller Found"));
		UE_LOG(LogTemp, Warning, TEXT("Moving to : %s"), *moveLocation.ToString());
		controlledChampion->GetController<AAC_PlayerAIController>()->setCachedMoveLocation(moveLocation); //Update the AIControllers cachecLocation
		controlledChampion->GetController<AAC_PlayerAIController>()->updateInternalState(MoveState); //Change the AIControllers state to the MoveState
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("AI Controller not Found")); //Can't find AIController
		if (controlledChampion == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("Controlled champion not Found either"));
		}
	}
	
	ClientUpdateNetAction("APC_ChampController::ServerMoveChampion_Implementation"); //Send back to client update
}
bool APC_ChampController::ServerMoveChampion_Validate(FVector moveLocation) { return true; }