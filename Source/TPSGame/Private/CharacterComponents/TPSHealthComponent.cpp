// Fill out your copyright notice in the Description page of Project Settings.

#include "TPSHealthComponent.h"
#include "Net/UnrealNetwork.h"


// Sets default values for this component's properties
UTPSHealthComponent::UTPSHealthComponent()
{
    // Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
    // off to improve performance if you don't need them.
    PrimaryComponentTick.bCanEverTick = true;
    DefaultHealth = 100.0f;
    SetIsReplicated(true);
    // ...
}


// Called when the game starts
void UTPSHealthComponent::BeginPlay()
{
    Super::BeginPlay();
    if (GetOwnerRole() == ROLE_Authority)
    {
        AActor* MyOwner = GetOwner();
        if (MyOwner)
        {
            MyOwner->OnTakeAnyDamage.AddDynamic(this, &UTPSHealthComponent::HandleTakeAnyDamage);
        }
    }
    
    Health = DefaultHealth;
}


// Called every frame
void UTPSHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    // ...
}

void UTPSHealthComponent::HandleTakeAnyDamage(AActor* DamagedActor, float  Damage, const class UDamageType*  DamageType, class AController*  InstigatedBy, AActor*  DamageCauser)
{
    if (Damage < 0)
    {
        UE_LOG(LogTemp, Log, TEXT("Damage error %s"), Damage);
        return;
    }
    Health = FMath::Clamp(Health - Damage, 0.0f, DefaultHealth);
    UE_LOG(LogTemp, Log, TEXT("UTPSHealthComponent, health change %s"), *FString::SanitizeFloat(Health));
    OnHealthChanged.Broadcast(this, DamagedActor, Health, Damage, DamageType, InstigatedBy, DamageCauser);
}

void UTPSHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    
    DOREPLIFETIME(UTPSHealthComponent, Health);
    
}

void UTPSHealthComponent::OnRep_Health(float OldHealth)
{
    float Damage = OldHealth - Health;
    OnHealthChanged.Broadcast(this, nullptr, Health, Damage, nullptr, nullptr, nullptr);
}

