// Copyright Sergei Shavrin 2024. All Rights Reserved.

#include "EnemyCharacter.h"

#include "Archons/Player/MainPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Runtime/AIModule/Classes/AIController.h"

AEnemyCharacter::AEnemyCharacter()
{
    PrimaryActorTick.bCanEverTick = false;

    AIControllerRef = nullptr;
    MainPlayerControllerRef = nullptr;

    Health = 20.0f;
}

void AEnemyCharacter::BeginPlay()
{
    Super::BeginPlay();

    MainPlayerControllerRef = Cast<AMainPlayerController>(UGameplayStatics::GetPlayerController(this, 0));
    ensureAlways(MainPlayerControllerRef);

    // Look in player's general direction
    if (IsValid(MainPlayerControllerRef))
    {
        const FRotator LookAtRotation{UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), MainPlayerControllerRef->GetPawn()->GetActorLocation())};

        FRotator Rotation{GetActorRotation()};
        Rotation.Yaw = LookAtRotation.Yaw;
        SetActorRotation(Rotation);
    }

    OnTakeAnyDamage.AddDynamic(this, &AEnemyCharacter::HandleTakeAnyDamage);

    OnSpawn();
}

void AEnemyCharacter::PossessedBy(AController* NewController)
{
    Super::PossessedBy(NewController);

    AIControllerRef = Cast<AAIController>(GetController());
    ensureAlways(AIControllerRef);
}

void AEnemyCharacter::OnSpawnAnimationComplete()
{
    GetWorldTimerManager().SetTimer(TargetTimerHandle, this, &AEnemyCharacter::PickTarget, 0.5f, true, 0.0f);
}

void AEnemyCharacter::OnHitAnimationCompete()
{
    GetWorldTimerManager().UnPauseTimer(TargetTimerHandle);
}

void AEnemyCharacter::OnDeathAnimationComplete()
{
    CharacterDiedDelegate.Broadcast();
    Destroy();
}

void AEnemyCharacter::PickTarget()
{
    if (!IsValid(MainPlayerControllerRef)) { return; }

    ACharacter* CharacterA{MainPlayerControllerRef->GetLeftCharacter()};
    ACharacter* CharacterB{MainPlayerControllerRef->GetRightCharacter()};

    if (!IsValid(CharacterA) || !IsValid(CharacterB)) { return; }

    const double DistanceA{FVector::Distance(GetActorLocation(), CharacterA->GetActorLocation())};
    const double DistanceB{FVector::Distance(GetActorLocation(), CharacterB->GetActorLocation())};

    CurrentTarget = (DistanceA < DistanceB) ? CharacterA : CharacterB;

    if (!IsValid(AIControllerRef)) { return; }

    AIControllerRef->MoveToActor(CurrentTarget.Get());
}

void AEnemyCharacter::HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
    if (Damage <= 0.0f || Health <= 0.0f) { return; }

    if (IsValid(AIControllerRef))
    {
        AIControllerRef->StopMovement();
    }

    if (Health > Damage)
    {
        Health -= Damage;
        GetWorldTimerManager().PauseTimer(TargetTimerHandle);
        OnHit();
    }
    else if (Health > 0.0f)
    {
        Health = 0.0f;
        GetWorldTimerManager().ClearTimer(TargetTimerHandle);
        OnDeath();
    }
}
