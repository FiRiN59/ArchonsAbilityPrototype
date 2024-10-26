// Copyright Sergei Shavrin 2024. All Rights Reserved.

#include "EnemySpawnerComponent.h"

#include "NavigationSystem.h"
#include "AI/NavigationSystemBase.h"
#include "Archons/Enemies/EnemyCharacter.h"
#include "Kismet/GameplayStatics.h"

UEnemySpawnerComponent::UEnemySpawnerComponent()
{
    PrimaryComponentTick.bCanEverTick = false;

    bDisabled = false;
    bShouldRespawn = false;
    SpawnRadius = 1000.0f;
}

void UEnemySpawnerComponent::BeginPlay()
{
    Super::BeginPlay();

    PlayerControllerRef = UGameplayStatics::GetPlayerController(this, 0);
    ensureAlways(PlayerControllerRef.IsValid());

    NavigationSystemRef = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
    ensureAlways(NavigationSystemRef.IsValid());
}

void UEnemySpawnerComponent::StartSpawning(const int32 NumberOfEnemies)
{
    if (bDisabled) { return; }

    bShouldRespawn = true;

    for (int32 Index = 0; Index < NumberOfEnemies; ++Index)
    {
        SpawnEnemy();
    }
}

void UEnemySpawnerComponent::StopSpawning()
{
    bShouldRespawn = false;
}

void UEnemySpawnerComponent::SpawnEnemy()
{
    if (!PlayerControllerRef.IsValid() || !NavigationSystemRef.IsValid()) { return; }

    const APawn* PlayerPawn{PlayerControllerRef->GetPawn()}; // PlayerPawn could be nullptr if the game is simulated in the editor
    const FVector SpawnCenter{PlayerPawn ? PlayerPawn->GetActorLocation() : FVector::ZeroVector};

    // Sometimes spawn can fail because of collisions, so we use a retry mechanism here.
    constexpr int32 MaxAttempts{5};
    for (int AttemptNumber = 1; AttemptNumber <= MaxAttempts; ++AttemptNumber)
    {
        FNavLocation SpawnLocation;
        if (NavigationSystemRef->GetRandomPointInNavigableRadius(SpawnCenter, SpawnRadius, SpawnLocation))
        {
            AEnemyCharacter* EnemyCharacter{GetWorld()->SpawnActor<AEnemyCharacter>(EnemyClass, SpawnLocation.Location, FRotator::ZeroRotator)};

            if (EnemyCharacter)
            {
                EnemyCharacter->SpawnDefaultController();
                EnemyCharacter->CharacterDiedDelegate.AddDynamic(this, &UEnemySpawnerComponent::HandleEnemyDeath);

                return;
            }

            UE_LOG(LogTemp, Warning, TEXT("Spawning enemy failed."))
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Unable to find a spawn location for an enemy."));
        }

        if (AttemptNumber <= MaxAttempts)
        {
            UE_LOG(LogTemp, Warning, TEXT("Trying again."));
        }
    }

    UE_LOG(LogTemp, Warning, TEXT("Abandoning spawn attempts after %i tries."), MaxAttempts);
}

void UEnemySpawnerComponent::HandleEnemyDeath()
{
    if (bShouldRespawn)
    {
        SpawnEnemy();
    }
}
