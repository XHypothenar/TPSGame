// Fill out your copyright notice in the Description page of Project Settings.

#include "TPSCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TPSHealthComponent.h"
#include "TPSWeapon.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ATPSCharacter::ATPSCharacter()
{
     // Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;
    CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
    
    
    SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
    SpringArmComp->SetupAttachment(RootComponent);
    SpringArmComp->bUsePawnControlRotation = true;
    CameraComp->SetupAttachment(SpringArmComp);
    DefaultViewField = CameraComp->FieldOfView;
    ViewFieldFOV = 70.0f;
    WeaponSocketName = "WeaponSocket";
    HealthComp = CreateDefaultSubobject<UTPSHealthComponent>(TEXT("HealthComp"));
    bSurvive = true;
    ZoomInterpSpeed = 20.0f;
	bSpeedUp = false;
	WalkInterpSpeed = 1.0f;
}

// Called when the game starts or when spawned
void ATPSCharacter::BeginPlay()
{
    Super::BeginPlay();
    DefaultViewField = CameraComp->FieldOfView;
    HealthComp->OnHealthChanged.AddDynamic(this, &ATPSCharacter::OnHealthChanged);
    if (Role == ROLE_Authority)
    {
        if (WeaponClass)
        {
            FActorSpawnParameters ActorSpawnParams;
            ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
            CurrentWeapon = GetWorld()->SpawnActor<ATPSWeapon>(WeaponClass, FVector::ZeroVector, FRotator::ZeroRotator, ActorSpawnParams);
            if (CurrentWeapon)
            {
                CurrentWeapon->SetOwner(this);
                CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponSocketName);
            }
        }
    }
    
    
}

// Called every frame
void ATPSCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    float TargetViewField;
    TargetViewField = bWantZoom? ViewFieldFOV:DefaultViewField;
    CurrentViewField = FMath::FInterpTo(CameraComp->FieldOfView, TargetViewField, DeltaTime, ZoomInterpSpeed);
    CameraComp->SetFieldOfView(CurrentViewField);
}

// Called to bind functionality to input
void ATPSCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
    PlayerInputComponent->BindAxis("MoveForward", this, &ATPSCharacter::MoveForward);
    PlayerInputComponent->BindAxis("MoveRight", this, &ATPSCharacter::MoveRight);
    PlayerInputComponent->BindAxis("LookUp", this, &ATPSCharacter::AddControllerPitchInput);
    PlayerInputComponent->BindAxis("Turn", this, &ATPSCharacter::AddControllerYawInput);
    
    GetMovementComponent()->GetNavAgentPropertiesRef().bCanCrouch = true;
    PlayerInputComponent->BindAction("Crouch",IE_Pressed, this, &ATPSCharacter::BeginCrouch);
    PlayerInputComponent->BindAction("Crouch",IE_Released, this, &ATPSCharacter::EndCrouch);
    
    PlayerInputComponent->BindAction("Jump",IE_Pressed, this, &ATPSCharacter::Jump);
    
    PlayerInputComponent->BindAction("Zoom",IE_Pressed, this, &ATPSCharacter::Zoom);
    PlayerInputComponent->BindAction("Zoom",IE_Released, this, &ATPSCharacter::EndZoom);
    
    PlayerInputComponent->BindAction("Fire",IE_Pressed, this, &ATPSCharacter::StartFire);
    PlayerInputComponent->BindAction("Fire",IE_Released, this, &ATPSCharacter::StopFire);

	PlayerInputComponent->BindAction("SpeedUp", IE_Pressed, this, &ATPSCharacter::SpeedUp);
	PlayerInputComponent->BindAction("SpeedUp", IE_Released, this, &ATPSCharacter::StopSpeedUp);

}

void ATPSCharacter::StartFire()
{
    if (CurrentWeapon)
    {
        CurrentWeapon->StartFire();
    }
    APawn* MyPawn = Cast<APawn>(this);
    if (MyPawn)
    {
        APlayerController* PC = Cast<APlayerController>(this->GetController());
        if (PC)
        {
            PC->ClientPlayCameraShake(CameraShake);
        }
    }
    
}

void ATPSCharacter::StopFire()
{
    if (CurrentWeapon)
    {
        CurrentWeapon->StopFire();
    }
}
void ATPSCharacter::MoveForward(float Value)
{
    AddMovementInput(GetActorForwardVector()* Value);
}

void ATPSCharacter::MoveRight(float Value)
{
    AddMovementInput(GetActorRightVector() * Value);
}

void ATPSCharacter::BeginCrouch()
{
    Crouch();
}

void ATPSCharacter::EndCrouch()
{
    UnCrouch();
}

void ATPSCharacter::BeginJump()
{
    Jump();
}

void ATPSCharacter::SpeedUp()
{

	UCharacterMovementComponent *MoveComp = GetCharacterMovement();
	MoveComp->MaxWalkSpeed = (MoveComp->MaxWalkSpeed) * 2;

}

void ATPSCharacter::StopSpeedUp()
{
	UCharacterMovementComponent *MoveComp = GetCharacterMovement();
	MoveComp->MaxWalkSpeed = (MoveComp->MaxWalkSpeed) / 2;
}

void ATPSCharacter::Zoom()
{
    bWantZoom = true;
}

void ATPSCharacter::EndZoom()
{
    bWantZoom = false;
}

void ATPSCharacter::OnHealthChanged(UTPSHealthComponent* HealthComponent, AActor* DamagedActor, float Health, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
    if (Health <= 0 && bSurvive == true)
    {
        GetMovementComponent()->StopMovementImmediately();
        bSurvive = false;
    }
    
}
FVector ATPSCharacter::GetPawnViewLocation() const
{
    if (CameraComp)
    {
        return CameraComp->GetComponentLocation();
    }
    return Super::GetPawnViewLocation();
}

void ATPSCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    
    DOREPLIFETIME(ATPSCharacter, CurrentWeapon);
    DOREPLIFETIME(ATPSCharacter, bSurvive);
    
}


