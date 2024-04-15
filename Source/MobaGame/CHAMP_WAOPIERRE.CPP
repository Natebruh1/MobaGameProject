//#include "D:\Users\NBurchell\Documents\Unreal Projects\MobaGame\Intermediate\Build\Win64\x64\MobaGameEditor\Development\Engine\SharedPCH.Engine.ShadowErrors.InclOrderUnreal5_1.h"
#include "champ_WaoPierre.h"

Achamp_WaoPierre::Achamp_WaoPierre()
{
	chosenChampion = FString("Wao Pierre");
	//Set base stats
	Ability1CD = 8.f;
	Ability3CD = 12.f;
	setUnitAttackRange(150.f); //Melee
	baseAttack = 140.f;

	bReplicates = true;
	//Fire = CreateDefaultSubobject<UNiagaraComponent>("Fire Particles");
	//Fire->Deactivate();
	FireBall = CreateDefaultSubobject<USphereComponent>(TEXT("Fireball Sphere")); //For overlapping
	FireBall->SetupAttachment(GetRootComponent());
	FireBall->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Overlap);
	FireBall->SetSphereRadius(16.f * baseSize);

	Fire = CreateDefaultSubobject<UNiagaraComponent>(TEXT("UNiagara Component Fire"));
	Fire->SetupAttachment(RootComponent);
	Fire->SetIsReplicated(true);
	


	FireSitIn = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Temp Mesh For Fire")); //For displaying
	FireSitIn->SetupAttachment(RootComponent);
	FireSitIn->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	//FireSitIn->SetHiddenInGame(true);

	//For UI
	secondaryName = FText::FromString("Stagger");
	Ability1Desc = FText::FromString("Breath fire in front of Wao Pierre");
	Ability2Desc = FText::FromString("Gain bonus attack temporarily. Gain 200 Stagger.");
	Ability3Desc = FText::FromString("Cut your staggered damage in half.");
}

void Achamp_WaoPierre::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	//STAGGER PASSIVE
	//3 Refers to howl long the damage is staggered for - 3 + 1% bAD. (So at 300 bonus AD, damage is dealt over 6 seconds).
	float staggerTick = staggeredDamage * (DeltaTime / (3+(bonusAttack*0.1f)));
	currentHealth -= staggerTick;
	staggeredDamage -= min(staggerTick, staggeredDamage);


	if (Ability1Active)
	{
		
		
		TArray<AActor*> result; //What we store overlapping actors in
		
		FVector ForwardVec = mouseVec; //Get the vector to point the fireball in
		ForwardVec.Normalize(); //and then normalize it
		FireBall->SetWorldLocation(this->GetActorLocation()+(ForwardVec*120.f)); //Set location to be slightly forwards
		FireSitIn->SetWorldLocation(this->GetActorLocation() + (ForwardVec * 120.f)); //Same with the displayed mesh
		
		FRotator a = mouseVec.Rotation(); //Get the rotation
		a.Pitch = 0.f; //Ignore pitch to lock it to the correct axis
		FireBall->SetWorldRotation(a);
		FireSitIn->SetWorldRotation(a);//Update the rotation for the mesh and the sphere component
		this->SetActorRotation(a);
		FireSitIn->SetVisibility(true); //Make the mesh visible
		
		FireBall->GetOverlappingActors(result, Achar_Unit::StaticClass()); //Get the actors
		UE_LOG(LogTemp, Warning, TEXT("Fireball Overlap : %d"), result.Num());
		for (int i = 0; i < result.Num(); i++)
		{
			if (result[i] != this && Cast<Achar_Unit>(result[i])->getUnitTeam()!=this->getUnitTeam()) //Cast the overlapping actor and check its on the opposite team
			{
				Achar_Unit* hitTarg = Cast<Achar_Unit>(result[i]); //If it is then deal damage to that target
				//Damage is 240 + 80% bAD over 1.2 seconds
				hitTarg->receiveDamage((240.f+(bonusAttack*0.8)) * GetWorld()->DeltaTimeSeconds); // 240 +80% bAD over the duration
				
			}
		}
	}

	secondaryPercentage = (staggeredDamage) / baseHealth; //Update the secondary percentage for UI feedback.
	if (secondaryPercentage < 0.005) { staggeredDamage = 0.f; } //Clamp the stagger value if it's low enough
}

void Achamp_WaoPierre::end_fire()
{
	//Turn the fire off
	Ability1Active = false;
	//Scale the mesh down
	FireSitIn->SetWorldScale3D(FVector(0.f, 0.f, 0.f));
}

void Achamp_WaoPierre::end_bonusAttack()
{
	bonusAttack -= 50; //Reduce the bonusAttack gained
}

//void Achamp_WaoPierre::AttackSteroidOnHit(Achar_Unit* waoPierre, Achar_Unit** target)
//{
//	(*target)->receiveDamage(500.f);
//}

void Achamp_WaoPierre::BeginPlay()
{
	Super::BeginPlay();
	
	//Fire->SetVisibility(false);
	Fire = UNiagaraFunctionLibrary::SpawnSystemAttached(FireComponent,
		GetRootComponent(), "", GetActorLocation(), GetActorRotation(), EAttachLocation::KeepRelativeOffset, true, true,
		ENCPoolMethod::AutoRelease, true); //Particles don't work across multiplayer, a helper class would have to be made to do this (for ease and compat with all champions/units)
	
	FireSitIn->SetWorldScale3D(FVector(0.f, 0.f, 0.f)); //Mesh
}

void Achamp_WaoPierre::receiveDamage(float val)
{
	//Overridden for passive effect
	float staggerPercent = 0.8-(bonusHealth*0.0005); //0.8 - 0.05 per 100 bonusHealth
	staggerPercent = max(0.f, staggerPercent);
	currentHealth -= val * staggerPercent; //Directly dealt damage
	staggeredDamage += val * (1 - staggerPercent); //Damage over time
	
}

void Achamp_WaoPierre::ability_1()
{
	UE_LOG(LogTemp, Warning, TEXT("Wao Pierre Ability 1"));
	
	//FireSitIn->SetWorldScale3D(FVector(1.f, 1.f, 2.f));
	

	//FireBall->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	
	if (!GetWorldTimerManager().IsTimerActive(Ability1Handle))
	{
		Ability1Active = true;
		GetWorldTimerManager().SetTimer(FireHandle, this, &Achamp_WaoPierre::end_fire, 1.2f, false);

		FireSitIn->SetWorldScale3D(FVector(2.f, 1.f, 1.f));
		GetWorldTimerManager().SetTimer(Ability1Handle, Ability1CD, false);
	}

}

void Achamp_WaoPierre::ability_1_Animation()
{
	
	
}

void Achamp_WaoPierre::ability_2()
{
	if (!GetWorldTimerManager().IsTimerActive(Ability2Handle))
	{
		FOnHitDelegate extraDmg;
		//Gain Bonus Attack Damage
		bonusAttack += 50.0;
		//Make the player take Damage in return
		staggeredDamage += 200.0;
		GetWorldTimerManager().SetTimer(AttackIncreaseHandle, this, &Achamp_WaoPierre::end_bonusAttack, 4.f, false); //Remove the bonus Attack in 4 seconds
		GetWorldTimerManager().SetTimer(Ability2Handle, Ability2CD, false);
		Achar_Unit* a = this;
		//extraDmg.BindUObject(this, &Achamp_WaoPierre::AttackSteroidOnHit, a, targetedUnitPtr);
	}
}

void Achamp_WaoPierre::ability_3()
{
	if (!GetWorldTimerManager().IsTimerActive(Ability3Handle))
	{
		staggeredDamage = staggeredDamage / 2; //Reduce Staggered Damage
		GetWorldTimerManager().SetTimer(Ability3Handle, Ability3CD, false);
	}
}




void Achamp_WaoPierre::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps)const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);


	//DOREPLIFETIME(Achar_BaseChampion, mouseVec);
	DOREPLIFETIME(Achamp_WaoPierre, staggeredDamage);
	
	//DOREPLIFETIME_CONDITION(APC_ChampController, cameraAttached, COND_OwnerOnly);
}