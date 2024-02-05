// Fill out your copyright notice in the Description page of Project Settings.


#include "char_BaseChampion.h"

void Achar_BaseChampion::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

Achar_BaseChampion::Achar_BaseChampion()
{
	//Set the size of the player capsule
	GetCapsuleComponent()->InitCapsuleSize(42.0f * getUnitSize(), 96.0f * getUnitSize());


	//Stop the character from rotating to the camera
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	//Setup Character Movement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 1000.0f, 0.0f); //How fast we rotate around the Y axis, increase if we need to rotate faster

	//Stick the player to the plane
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	//Setup spring arm or camera boom
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("Camera Springarm"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(true); //Stop the springarm from rotating with the players rotation
	CameraBoom->TargetArmLength = 1200.0f;
	CameraBoom->bDoCollisionTest = false;//Stop it from colliding with the rest of the level (can cause clipping)
	CameraBoom->SetRelativeRotation(FRotator(-60.f, 0.f, 0.f));//Rotate it facing upwards so its a top down camera


	//Now setup the camera (WIP, need to unlock and slide the camera along)
	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Top Down Camera"));
	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	//separate camera and spring arm rotation
	TopDownCameraComponent->bUsePawnControlRotation = false;



	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

}

void Achar_BaseChampion::ability_1()
{
	UE_LOG(LogTemp, Warning, TEXT("Base Champion Ability_1"));
}

void Achar_BaseChampion::ability_1_Animation()
{
	UE_LOG(LogTemp, Warning, TEXT("Base Champion Ability_1_Animation"));
}

void Achar_BaseChampion::ability_2()
{
	UE_LOG(LogTemp, Warning, TEXT("Base Champion Ability_2"));
}

void Achar_BaseChampion::ability_2_Animation()
{
	UE_LOG(LogTemp, Warning, TEXT("Base Champion Ability_2_Animation"));
}

void Achar_BaseChampion::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}






template<typename ...T>
ChampionScripts<T...>* Achar_BaseChampion::get_scripts()
{
	return &(static_cast<ChampionScripts<T...>>(scripts));
}


//void Achar_BaseChampion::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps)const
//{
//	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
//	
//
//
//	//DOREPLIFETIME_CONDITION(APC_ChampController, cameraAttached, COND_OwnerOnly);
//}