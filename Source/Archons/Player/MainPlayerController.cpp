// Copyright Sergei Shavrin 2024. All Rights Reserved.

#include "MainPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "MainPlayerCamera.h"
#include "Archons/Abilities/StringAbilityComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

AMainPlayerController::AMainPlayerController()
{
    StringAbilityComponent = CreateDefaultSubobject<UStringAbilityComponent>(TEXT("StringAbility"));

    MainPlayerCameraRef = nullptr;

    InputMappingContext = nullptr;
    MoveLeftCharacterAction = nullptr;
    MoveRightCharacterAction = nullptr;

    LeftCharacterRef = nullptr;
    RightCharacterRef = nullptr;
}

void AMainPlayerController::BeginPlay()
{
    Super::BeginPlay();

    MainPlayerCameraRef = Cast<AMainPlayerCamera>(GetPawn());
    ensureAlways(MainPlayerCameraRef);

    LeftCharacterRef = Cast<ACharacter>(UGameplayStatics::GetActorOfClass(this, LeftCharacterClass));
    RightCharacterRef = Cast<ACharacter>(UGameplayStatics::GetActorOfClass(this, RightCharacterClass));
    ensureAlwaysMsgf(LeftCharacterRef, TEXT("Character with class %s was not found in the level"), LeftCharacterClass ? *LeftCharacterClass->GetName() : TEXT("None"));
    ensureAlwaysMsgf(RightCharacterRef, TEXT("Character with class %s was not found in the level"), RightCharacterClass ? *RightCharacterClass->GetName() : TEXT("None"));

    StringAbilityComponent->ActivateAbility();
}

void AMainPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    if (UEnhancedInputLocalPlayerSubsystem* InputSubsystem{ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer())})
    {
        InputSubsystem->AddMappingContext(InputMappingContext, 0);
    }

    if (UEnhancedInputComponent* EnhancedInputComponent{Cast<UEnhancedInputComponent>(InputComponent)})
    {
        EnhancedInputComponent->BindAction(MoveLeftCharacterAction, ETriggerEvent::Triggered, this, &AMainPlayerController::OnMoveLeftCharacter_Triggered);
        EnhancedInputComponent->BindAction(MoveRightCharacterAction, ETriggerEvent::Triggered, this, &AMainPlayerController::OnMoveRightCharacter_Triggered);
    }
}

void AMainPlayerController::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    if (!IsValid(LeftCharacterRef) || !IsValid(RightCharacterRef) || !IsValid(MainPlayerCameraRef)) { return; }

    FVector CameraPos = UKismetMathLibrary::VLerp(LeftCharacterRef->GetActorLocation(), RightCharacterRef->GetActorLocation(), 0.5f);
    CameraPos.Z = MainPlayerCameraRef->GetActorLocation().Z;
    MainPlayerCameraRef->SetActorLocation(CameraPos);

    const FVector Direction = RightCharacterRef->GetActorLocation() - LeftCharacterRef->GetActorLocation();
    const FRotator Rotation = UKismetMathLibrary::MakeRotFromYZ(Direction, FVector::ZAxisVector);
    MainPlayerCameraRef->SetActorRotation(Rotation);
}

void AMainPlayerController::OnMoveLeftCharacter_Triggered(const FInputActionValue& InputActionValue)
{
    if (!IsValid(LeftCharacterRef) || !IsValid(MainPlayerCameraRef)) { return; }

    const FVector2D InputValue{InputActionValue.Get<FVector2D>()};
    const FVector InputDirection{InputValue.X, InputValue.Y, 0.0};

    // Adjust input direction in relation to current camera position
    const FVector MovementDirection{InputDirection.RotateAngleAxis(MainPlayerCameraRef->GetCameraRotation().Yaw, FVector::ZAxisVector)};

    LeftCharacterRef->AddMovementInput(MovementDirection);
}

void AMainPlayerController::OnMoveRightCharacter_Triggered(const FInputActionValue& InputActionValue)
{
    if (!IsValid(RightCharacterRef) || !IsValid(MainPlayerCameraRef)) { return; }

    const FVector2D InputValue{InputActionValue.Get<FVector2D>()};
    const FVector InputDirection{InputValue.X, InputValue.Y, 0.0};

    // Adjust input direction in relation to current camera position
    const FVector MovementDirection{InputDirection.RotateAngleAxis(MainPlayerCameraRef->GetCameraRotation().Yaw, FVector::ZAxisVector)};

    RightCharacterRef->AddMovementInput(MovementDirection);
}

bool AMainPlayerController::GetAbilitySpan(FVector& OutPointA, FVector& OutPointB) const
{
    if (!IsValid(LeftCharacterRef) || !IsValid(RightCharacterRef)) { return false; }

    OutPointA = LeftCharacterRef->GetMovementComponent()->GetActorFeetLocation();
    OutPointB = RightCharacterRef->GetMovementComponent()->GetActorFeetLocation();

    const FVector DirectionAB{(OutPointB - OutPointA).GetSafeNormal()};

    OutPointA += DirectionAB * LeftCharacterRef->GetCapsuleComponent()->GetScaledCapsuleRadius();
    OutPointB -= DirectionAB * RightCharacterRef->GetCapsuleComponent()->GetScaledCapsuleRadius();

    return true;
}

void AMainPlayerController::GetIgnoreDamageActors(TArray<AActor*>& OutActors) const
{
    OutActors.Add(LeftCharacterRef);
    OutActors.Add(RightCharacterRef);
}

ACharacter* AMainPlayerController::GetLeftCharacter() const
{
    return LeftCharacterRef;
}

ACharacter* AMainPlayerController::GetRightCharacter() const
{
    return RightCharacterRef;
}
