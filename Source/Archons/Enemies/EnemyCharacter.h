// Copyright Sergei Shavrin 2024. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnemyCharacter.generated.h"

class AMainPlayerController;
class AAIController;
class AEnemyCharacter;

DECLARE_DYNAMIC_MULTICAST_SPARSE_DELEGATE(FEnemyCharacterDiedDelegate, AEnemyCharacter, CharacterDiedDelegate);

UCLASS()
class ARCHONS_API AEnemyCharacter : public ACharacter
{
    GENERATED_BODY()

    UPROPERTY()
    AAIController* AIControllerRef;

    UPROPERTY()
    AMainPlayerController* MainPlayerControllerRef;

    UPROPERTY(EditAnywhere)
    float Health;

public:
    AEnemyCharacter();

    virtual void PossessedBy(AController* NewController) override;

    FEnemyCharacterDiedDelegate CharacterDiedDelegate;

protected:
    virtual void BeginPlay() override;

    UFUNCTION(BlueprintImplementableEvent)
    void OnSpawn();

    UFUNCTION(BlueprintImplementableEvent)
    void OnHit();

    UFUNCTION(BlueprintImplementableEvent)
    void OnDeath();

    UFUNCTION(BlueprintCallable)
    void OnSpawnAnimationComplete();

    UFUNCTION(BlueprintCallable)
    void OnHitAnimationCompete();

    UFUNCTION(BlueprintCallable)
    void OnDeathAnimationComplete();

    UFUNCTION()
    void HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

private:
    FTimerHandle TargetTimerHandle;
    TWeakObjectPtr<ACharacter> CurrentTarget;

    void PickTarget();
};
