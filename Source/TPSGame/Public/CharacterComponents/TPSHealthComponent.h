// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TPSHealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_SevenParams(FOnHealthChanged, UTPSHealthComponent*, HealthComp, AActor*, DamagedActor, float, Health, float, Damage, const class UDamageType*, DamageType, class AController*, InstigatedBy, AActor*, DamageCauser);

UCLASS( ClassGroup=(TPS), meta=(BlueprintSpawnableComponent) )
class TPSGAME_API UTPSHealthComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    // Sets default values for this component's properties
    UTPSHealthComponent();

protected:
    // Called when the game starts
    virtual void BeginPlay() override;
    
    UPROPERTY(ReplicatedUsing = OnRep_Health, EditDefaultsOnly, BlueprintReadOnly, Category = "HealthComponent")
    float Health;
    
    UFUNCTION()
    void OnRep_Health(float OldHealth);
    
    UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "HealthComponent")
    float DefaultHealth;
    
public:
    // Called every frame
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
    
    UFUNCTION()
    void HandleTakeAnyDamage(AActor* DamagedActor, float  Damage, const class UDamageType*  DamageType, class AController*  InstigatedBy, AActor*  DamageCauser);
    
    UPROPERTY(BlueprintAssignable,Category = "Events")
    FOnHealthChanged OnHealthChanged;
};

