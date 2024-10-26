// Copyright Sergei Shavrin 2024. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EnemySpawnerComponent.generated.h"

class UNavigationSystemV1;
class AEnemyCharacter;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ARCHONS_API UEnemySpawnerComponent : public UActorComponent
{
    GENERATED_BODY()

    bool bShouldRespawn;

    TWeakObjectPtr<APlayerController> PlayerControllerRef;
    TWeakObjectPtr<UNavigationSystemV1> NavigationSystemRef;

    UPROPERTY(EditDefaultsOnly, DisplayName="Disabled?")
    bool bDisabled;

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<AEnemyCharacter> EnemyClass;

    UPROPERTY(EditAnywhere)
    float SpawnRadius;

public:
    UEnemySpawnerComponent();

protected:
    virtual void BeginPlay() override;

public:
    UFUNCTION(BlueprintCallable)
    void StartSpawning(const int32 NumberOfEnemies);

    UFUNCTION(BlueprintCallable)
    void StopSpawning();

private:
    void SpawnEnemy();

    UFUNCTION()
    void HandleEnemyDeath();
};
