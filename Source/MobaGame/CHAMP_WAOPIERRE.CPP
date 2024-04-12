//#include "D:\Users\NBurchell\Documents\Unreal Projects\MobaGame\Intermediate\Build\Win64\x64\MobaGameEditor\Development\Engine\SharedPCH.Engine.ShadowErrors.InclOrderUnreal5_1.h"
#include "champ_WaoPierre.h"

Achamp_WaoPierre::Achamp_WaoPierre()
{
	chosenChampion = FString("Wao Pierre");
	//Set base stats
	Ability1CD = 8.f;
	Ability3CD = 12.f;
	setUnitAttackRange(1024.f);
	baseAttack = 300.f;

	bReplicates = true;
	//Fire = CreateDefaultSubobject<UNiagaraComponent>("Fire Particles");
	//Fire->Deactivate();
	FireBall = CreateDefaultSubobject<USphereComponent>(TEXT("Fireball Sphere"));
	FireBall->SetupAttachment(GetRootComponent());
	FireBall->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Overlap);
	FireBall->SetSphereRadius(16.f * baseSize);

	Fire = CreateDefaultSubobject<UNiagaraComponent>(TEXT("UNiagara Component Fire"));
	Fire->SetupAttachment(RootComponent);
	Fire->SetIsReplicated(true);
	


	FireSitIn = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Temp Mesh For Fire"));
	FireSitIn->SetupAttachment(RootComponent);
	FireSitIn->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	//FireSitIn->SetHiddenInGame(true);
	secondaryName = FText::FromString("Stagger");
	Ability1Desc = FText::FromString("Breath fire in front of Wao Pierre");
	Ability2Desc = FText::FromString("Gain bonus attack temporarily. Gain 200 Staggered damage.");
	Ability3Desc = FText::FromString("Cut your staggered damage in half.");
}

void Achamp_WaoPierre::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	//STAGGER PASSIVE
	//3 Refers to howl long the damage is staggered for
	float staggerTick = staggeredDamage * (DeltaTime / 3);
	currentHealth -= staggerTick;
	staggeredDamage -= min(staggerTick, staggeredDamage);


	if (Ability1Active)
	{
		
		//UE_LOG(LogTemp, Warning, TEXT("Wao Pierre Shooting Fireball"));
		TArray<AActor*> result;
		UE_LOG(LogTemp, Warning, TEXT("MVec : %s"), *mouseVec.ToString());
		FVector ForwardVec = mouseVec;
		ForwardVec.Normalize();
		FireBall->SetWorldLocation(this->GetActorLocation()+(ForwardVec*120.f));
		FireSitIn->SetWorldLocation(this->GetActorLocation() + (ForwardVec * 120.f));
		
		FRotator a = mouseVec.Rotation();
		a.Pitch = 0.f;
		FireBall->SetWorldRotation(a);
		FireSitIn->SetWorldRotation(a);
		this->SetActorRotation(a);
		FireSitIn->SetVisibility(true);
		
		FireBall->GetOverlappingActors(result, Achar_Unit::StaticClass());
		UE_LOG(LogTemp, Warning, TEXT("Fireball Overlap : %d"), result.Num());
		for (int i = 0; i < result.Num(); i++)
		{
			if (result[i] != this && Cast<Achar_Unit>(result[i])->getUnitTeam()!=this->getUnitTeam())
			{
				Achar_Unit* hitTarg = Cast<Achar_Unit>(result[i]);
				//Damage is 240 + 80% bAD over 1.2 seconds
				hitTarg->receiveDamage((240.f+(bonusAttack*0.8)) * GetWorld()->DeltaTimeSeconds);
				
			}
		}
	}

	secondaryPercentage = (staggeredDamage) / baseHealth;
	if (secondaryPercentage < 0.005) { staggeredDamage = 0.f; }
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
	bonusAttack -= 50;
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
		ENCPoolMethod::AutoRelease, true);
	//Fire = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), FireComponent, FireBall->GetComponentLocation());
	FireSitIn->SetWorldScale3D(FVector(0.f, 0.f, 0.f));
}

void Achamp_WaoPierre::receiveDamage(float val)
{
	//Overridden for passive effect
	float staggerPercent = 0.8;
	currentHealth -= val * staggerPercent;
	staggeredDamage += val * (1 - staggerPercent);
	//ability_1();
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