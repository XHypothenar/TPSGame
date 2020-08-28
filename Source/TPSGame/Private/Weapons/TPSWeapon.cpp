// Fill out your copyright notice in the Description page of Project Settings.

#include "TPSWeapon.h"
#include "DrawDebugHelpers.h"
#include "kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "TimerManager.h"
#include "TPSGame.h"
#include "Net/UnrealNetwork.h"

static int32 DebugWeaponsDraw = 0;
FAutoConsoleVariableRef CVARDebugWeaponsDrawing(
                                                TEXT("COOP.DebugWeapons"),
                                                DebugWeaponsDraw,
                                                TEXT("Draw Debug Lines For Weapons!"),
                                                ECVF_Cheat);




// Sets default values
ATPSWeapon::ATPSWeapon()
{
     // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;
    MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComp"));
    RootComponent = MeshComp;
    MuzzleSocketName = "MuzzleSocket";
    TraceTargetName = "Target";
    DamageBase = 20.0f;
    RateOfFire= 600;
    SetReplicates(true);
}

// Called when the game starts or when spawned
void ATPSWeapon::BeginPlay()
{
    Super::BeginPlay();
    TimeBetweenFire = 60 / RateOfFire;
}

// Called every frame
void ATPSWeapon::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

}

void ATPSWeapon::StartFire()
{
    float DelayTime = FMath::Max(LastFireTime + TimeBetweenFire - GetWorld()->TimeSeconds, 0.0f);
    GetWorld()->GetTimerManager().SetTimer(TimeHandel, this, &ATPSWeapon::Fire, TimeBetweenFire, true, DelayTime);
}

void ATPSWeapon::StopFire()
{
    GetWorld()->GetTimerManager().ClearTimer(TimeHandel);
}
void ATPSWeapon::Fire()
{
    if (Role < ROLE_Authority)
    {
        ServerFire();
    }
    AActor* MyOwner = GetOwner();
    if (MyOwner)
    {
        FVector EyeLocation;
        FRotator EyeRotation;
        MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);
        FVector ShootDirection = EyeRotation.Vector();
        FVector TraceEnd = EyeLocation + (ShootDirection * 10000);
        FCollisionQueryParams QueryParms;
        QueryParms.AddIgnoredActor(MyOwner);
        QueryParms.AddIgnoredActor(this);
        QueryParms.bTraceComplex = true;
        QueryParms.bReturnPhysicalMaterial = true;
        FVector TraceEndPoint = TraceEnd;
        FHitResult Hit;
        EPhysicalSurface SurfaceType = SURFACE_FLESH_DEFAULTS;
        if (GetWorld()->LineTraceSingleByChannel(Hit, EyeLocation, TraceEnd, ECC_Visibility, QueryParms))
        {
            AActor* HitActor = Hit.GetActor();
            float ActualDamage;
            SurfaceType = UPhysicalMaterial::DetermineSurfaceType(Hit.PhysMaterial.Get());
            switch (SurfaceType) {
                case SURFACE_FLESH_VULNERABLE:
                    ActualDamage = DamageVulnerable;
                    break;
                    
                default:
                    ActualDamage = DamageBase;
                    break;
            }
            UGameplayStatics::ApplyPointDamage(HitActor, ActualDamage, ShootDirection, Hit, MyOwner->GetInstigatorController(), this, DamageType);
            
            PlayFireEffects(Hit.ImpactPoint);
            PlayImpactEffects(SurfaceType, Hit.ImpactPoint);
            TraceEndPoint = Hit.ImpactPoint;
            
            PlayFireEffects(TraceEndPoint);
            
            if (Role == ROLE_Authority)
            {
                HitScanTrace.TraceTo = TraceEndPoint;
                HitScanTrace.SurfaceType = SurfaceType;
            }
        
            
        }
        if (DebugWeaponsDraw > 0)
        {
            DrawDebugLine(GetWorld(), EyeLocation, TraceEnd, FColor::White, false, 1.0f, 0, 1.0f);
        }
        
       
        LastFireTime = GetWorld()->TimeSeconds;
        
        
    }
}
void ATPSWeapon::PlayImpactEffects(EPhysicalSurface SurfaceType, FVector Impactpoint)
{
    UParticleSystem* SelectEffect;
    switch(SurfaceType)
    {
        case SURFACE_FLESH_DEFAULTS:
        case SURFACE_FLESH_VULNERABLE:
            SelectEffect = ImpactEffect;
            break;
        default:
            SelectEffect = DefaultFleshEffect;
            break;
    }
    if (SelectEffect)
    {
        FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);
        FVector ShootDirection = Impactpoint - MuzzleLocation;
        ShootDirection.Normalize();
        UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SelectEffect, Impactpoint, ShootDirection.Rotation());
        
    }
}

void ATPSWeapon::ServerFire_Implementation()
{
    Fire();
}

bool ATPSWeapon::ServerFire_Validate()
{
    return true;
}
void ATPSWeapon::PlayFireEffects(FVector TraceEndPoint)
{
    if (MuzzleEffect)
    {
        UGameplayStatics::SpawnEmitterAttached(MuzzleEffect, MeshComp, MuzzleSocketName);
    }
    
    if (TraceEffect)
    {
        FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);
        UParticleSystemComponent* TraceComp = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), TraceEffect, MuzzleLocation);
        if (TraceComp)
        {
            TraceComp->SetVectorParameter(TraceTargetName, TraceEndPoint);
        }
    }
}

void ATPSWeapon::OnRep_HitScanTrace()
{
    PlayFireEffects(HitScanTrace.TraceTo);
    PlayImpactEffects(HitScanTrace.SurfaceType, HitScanTrace.TraceTo);
}

void ATPSWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME_CONDITION(ATPSWeapon, HitScanTrace, COND_SkipOwner);
}
