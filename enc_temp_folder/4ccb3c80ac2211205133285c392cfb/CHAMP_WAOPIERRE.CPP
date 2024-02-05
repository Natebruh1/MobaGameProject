#include "D:\Users\NBurchell\Documents\Unreal Projects\MobaGame\Intermediate\Build\Win64\x64\MobaGameEditor\Development\Engine\SharedPCH.Engine.ShadowErrors.InclOrderUnreal5_1.h"
#include "champ_WaoPierre.h"

Achamp_WaoPierre::Achamp_WaoPierre()
{
	//Set base stats
	Ability1CD = 8.f;
	Ability2CD = 12.f;
	setUnitAttackRange(128.f);
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
		FireBall->SetWorldLocation(this->GetActorLocation()+(this->GetActorForwardVector()*180.f));
		FireSitIn->SetWorldLocation(this->GetActorLocation() + (this->GetActorForwardVector() * 120.f));
		FireSitIn->SetVisibility(true);
		
		FireBall->GetOverlappingActors(result, Achar_Unit::StaticClass());
		UE_LOG(LogTemp, Warning, TEXT("Fireball Overlap : %d"), result.Num());
		for (int i = 0; i < result.Num(); i++)
		{
			if (result[i] != this && Cast<Achar_Unit>(result[i])->getUnitTeam()!=this->getUnitTeam())
			{
				Achar_Unit* hitTarg = Cast<Achar_Unit>(result[i]);
				hitTarg->receiveDamage(150.f * GetWorld()->DeltaTimeSeconds);
			}
		}
	}

}

void Achamp_WaoPierre::end_fire()
{
	//Turn the fire off
	Ability1Active = false;
	//Scale the mesh down
	FireSitIn->SetWorldScale3D(FVector(0.f, 0.f, 0.f));
}

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
		staggeredDamage = staggeredDamage / 2;
		GetWorldTimerManager().SetTimer(Ability2Handle, Ability2CD, false);
	}
}


//void Achamp_WaoPierre::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps)const
//{
//	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
//	DOREPLIFETIME(Achamp_WaoPierre, Fire);
//	
//
//	//DOREPLIFETIME_CONDITION(APC_ChampController, cameraAttached, COND_OwnerOnly);
//}