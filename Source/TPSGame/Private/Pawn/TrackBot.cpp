// Fill out your copyright notice in the Description page of Project Settings.


#include "TrackBot.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"
#include "GameFramework/Character.h"
#include "DrawDebugHelpers.h"
#include "TPSHealthComponent.h"

// Sets default values
ATrackBot::ATrackBot()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetCanEverAffectNavigation(false);
	RootComponent = MeshComp;

	HealthComp = CreateDefaultSubobject<UTPSHealthComponent>(TEXT("HealthComp"));
	HealthComp->OnHealthChanged.AddDynamic(this, &ATrackBot::HandleTakeDamage);
	MovementForce = 1000;
	bUseVelocityChange = false;
	RequiredDistance = 100;
	BeExploded = false;
	ExpolsionDamage = 100;

}

// Called when the game starts or when spawned
void ATrackBot::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATrackBot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	float Distance = (GetActorLocation() - NextPathPoint).Size();
	if (Distance > RequiredDistance)
	{
		FVector ForceDirection = NextPathPoint - GetActorLocation();
		ForceDirection.Normalize();
		ForceDirection *= MovementForce;
		MeshComp->AddForce(ForceDirection, NAME_None, bUseVelocityChange);
		DrawDebugDirectionalArrow(GetWorld(), GetActorLocation(), GetActorLocation() + ForceDirection, 32, FColor::Yellow, false, 0.0f, 0, 1.0f);
	}
	else
	{
		NextPathPoint = GetNextPathPoint();
	}
	DrawDebugSphere(GetWorld(), NextPathPoint, 20, 12, FColor::Yellow, false, 0.0f, 1.0f);
}

// Called to bind functionality to input
void ATrackBot::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

FVector ATrackBot::GetNextPathPoint()
{
	ACharacter * MyPlayer = UGameplayStatics::GetPlayerCharacter(this, 0);
	UNavigationPath * NavPath = UNavigationSystemV1::FindPathToActorSynchronously(this, GetActorLocation(), MyPlayer);
	if (NavPath->PathPoints.Num() > 1)
	{
		return NavPath->PathPoints[1];
	}
	return GetActorLocation();
}

void ATrackBot::HandleTakeDamage(UTPSHealthComponent * HealthComponent, AActor * DamagedActor, float Health, float Damage, const UDamageType * DamageType, AController * InstigatedBy, AActor * DamageCauser)
{
	if (MatInst == nullptr)
	{
		MatInst = MeshComp->CreateAndSetMaterialInstanceDynamicFromMaterial(0, MeshComp->GetMaterial(0));
	}
	if (MatInst)
	{
		MatInst->SetScalarParameterValue("LastTimeDamageTaken", GetWorld()->TimeSeconds);
	}
	if (Health <= 0.0f)
	{
		SelfDestruct();
	}

	UE_LOG(LogTemp, Log, TEXT("ATrackBot Health change %s"), *FString::SanitizeFloat(Health));
}

void ATrackBot::SelfDestruct()
{
	if (BeExploded)
	{
		return;
	}
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplossionEffect, GetActorLocation());
	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(this);
	UGameplayStatics::ApplyRadialDamage(this, 100, GetActorLocation(), 200, nullptr, IgnoreActors, this, GetInstigatorController(), true);
	DrawDebugSphere(GetWorld(), NextPathPoint, 20, 12, FColor::Yellow, false, 0.0f, 1.0f);
	Destroy();
	BeExploded = true;
}

