// Copyright Sergei Shavrin 2024. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Archons/Interfaces/SpanAbilityOwner.h"
#include "GameFramework/PlayerController.h"
#include "MainPlayerController.generated.h"

class UStringAbilityComponent;
struct FInputActionValue;
class UInputAction;
class AMainPlayerCamera;
class UInputMappingContext;

UCLASS()
class ARCHONS_API AMainPlayerController : public APlayerController, public ISpanAbilityOwner
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly, Category="Input")
    UInputMappingContext* InputMappingContext;

    UPROPERTY(EditDefaultsOnly, Category="Input")
    UInputAction* MoveLeftCharacterAction;

    UPROPERTY(EditDefaultsOnly, Category="Input")
    UInputAction* MoveRightCharacterAction;

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<ACharacter> LeftCharacterClass;

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<ACharacter> RightCharacterClass;

    UPROPERTY(VisibleAnywhere)
    ACharacter* LeftCharacterRef;

    UPROPERTY(VisibleAnywhere)
    ACharacter* RightCharacterRef;

public:
    AMainPlayerController();

    virtual void Tick(float DeltaSeconds) override;

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UStringAbilityComponent* StringAbilityComponent;

    UPROPERTY(VisibleAnywhere)
    AMainPlayerCamera* MainPlayerCameraRef;

    virtual void BeginPlay() override;
    virtual void SetupInputComponent() override;

private:
    void OnMoveLeftCharacter_Triggered(const FInputActionValue& InputActionValue);
    void OnMoveRightCharacter_Triggered(const FInputActionValue& InputActionValue);

public:
    /** ISpanAbilityOwner */
    virtual bool GetAbilitySpan(FVector& OutPointA, FVector& OutPointB) const override;
    virtual void GetIgnoreDamageActors(TArray<AActor*>& OutActors) const override;

    /** Getters and Setters */
    ACharacter* GetLeftCharacter() const;
    ACharacter* GetRightCharacter() const;
};
