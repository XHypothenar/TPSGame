// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TPSCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
class ATPSWeapon;
class UTPSHealthComponent;


UCLASS()
class TPSGAME_API ATPSCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    // Sets default values for this character's properties
    ATPSCharacter();

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UCameraComponent* CameraComp;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    USpringArmComponent* SpringArmComp;
    
    float DefaultViewField;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player")
    float ViewFieldFOV;
    
    UPROPERTY(EditDefaultsOnly, Category="Player")
    TSubclassOf<ATPSWeapon> WeaponClass;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player",  meta = (ClampMin = 0.1, ClampMax = 100.0))
    float ZoomInterpSpeed;
    
    UPROPERTY(EditDefaultsOnly, Category="Player")
    TSubclassOf<UCameraShake> CameraShake;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
    UTPSHealthComponent* HealthComp;
    
    UPROPERTY(Replicated, BlueprintReadOnly, Category="Player")
    bool bSurvive;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player", meta = (ClampMin = 1.0f, ClampMax = 10.0f))
	float WalkInterpSpeed;
    
    UFUNCTION()
    void OnHealthChanged(UTPSHealthComponent* HealthComponent, AActor* DamagedActor, float Health, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);
    
    float CurrentViewField;
    
    bool bWantZoom;
    
	bool bSpeedUp;
    
    UPROPERTY(Replicated)
    ATPSWeapon *CurrentWeapon;
    
    FName WeaponSocketName;
    
public:
    // Called every frame
    virtual void Tick(float DeltaTime) override;

    // Called to bind functionality to input
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
    
    virtual FVector GetPawnViewLocation() const override;
    
    UFUNCTION()
    void MoveForward(float Value);
    
    UFUNCTION()
    void MoveRight(float Value);
    
    UFUNCTION()
    void BeginCrouch();
    
    UFUNCTION()
    void EndCrouch();
    
    UFUNCTION()
    void BeginJump();

	UFUNCTION()
	void SpeedUp();

	UFUNCTION()
	void StopSpeedUp();

    void Zoom();
    
    void EndZoom();
    
    void StartFire();
    
    void StopFire();
	
    
    
};
