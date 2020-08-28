// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "TrackBot.generated.h"


class UTPSHealthComponent;

UCLASS()
class TPSGAME_API ATrackBot : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ATrackBot();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	UStaticMeshComponent *MeshComp;
	
	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	UTPSHealthComponent *HealthComp;

	FVector NextPathPoint;

	UPROPERTY(EditDefaultsOnly, Category = "TrackBot")
	float MovementForce;

	UPROPERTY(EditDefaultsOnly, Category = "TrackBot")
	bool bUseVelocityChange;

	UPROPERTY(EditDefaultsOnly, Category = "TrackBot")
	float RequiredDistance;

	UMaterialInstanceDynamic *MatInst;

	UPROPERTY(EditDefaultsOnly, Category = "TrackBot")
	UParticleSystem *ExplossionEffect;

	UPROPERTY(EditDefaultsOnly, Category = "TrackBot")
	float ExpolsionRadial;

	UPROPERTY(EditDefaultsOnly, Category = "TrackBot")
	float ExpolsionDamage;

	bool BeExploded;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION()
	FVector GetNextPathPoint();

	UFUNCTION()
	void HandleTakeDamage(UTPSHealthComponent* HealthComponent, AActor* DamagedActor, float Health, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	void SelfDestruct();
};
