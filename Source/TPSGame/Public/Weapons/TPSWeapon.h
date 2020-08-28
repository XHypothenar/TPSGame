// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TPSWeapon.generated.h"


class USkeletalMeshComponent;
class UDamageType;
class UParticleSystem;


USTRUCT()
struct FHitScanTrace
{
    GENERATED_BODY()
public:
    UPROPERTY()
    TEnumAsByte<EPhysicalSurface> SurfaceType;
    
    UPROPERTY()
    FVector_NetQuantize TraceTo;
};


UCLASS()
class TPSGAME_API ATPSWeapon : public AActor
{
    GENERATED_BODY()
    
public:
    // Sets default values for this actor's properties
    ATPSWeapon();

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "components")
    USkeletalMeshComponent* MeshComp;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
    TSubclassOf<UDamageType> DamageType;
    
    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
    FName MuzzleSocketName;
    
    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
    FName TraceTargetName;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
    UParticleSystem* MuzzleEffect;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
    UParticleSystem* ImpactEffect;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
    UParticleSystem* DefaultFleshEffect;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
    UParticleSystem* TraceEffect;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
    float DamageBase;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
    float DamageVulnerable;
    
    FTimerHandle TimeHandel;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
    float RateOfFire;
    
    int LastFireTime;
    
    float TimeBetweenFire;
    
    UPROPERTY(ReplicatedUsing = OnRep_HitScanTrace)
    FHitScanTrace HitScanTrace;
    
    UFUNCTION()
    void OnRep_HitScanTrace();
    
public:
    // Called every frame
    virtual void Tick(float DeltaTime) override;
    
    UFUNCTION(BlueprintCallable, Category = "Weapon")
    virtual void Fire();
    
    virtual void StartFire();
    
    virtual void StopFire();
    
    void PlayFireEffects(FVector TraceEndPoint);
    
    void PlayImpactEffects(EPhysicalSurface SurfaceType, FVector Impactpoint);
    
    UFUNCTION(Server, Reliable, WithValidation)
    void ServerFire();
};
