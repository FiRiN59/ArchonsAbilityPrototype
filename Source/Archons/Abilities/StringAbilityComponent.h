// Copyright Sergei Shavrin 2024. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StringAbilityComponent.generated.h"

class ISpanAbilityOwner;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ARCHONS_API UStringAbilityComponent : public UActorComponent
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, DisplayName="Debug?")
    bool bDebug;

    TWeakInterfacePtr<ISpanAbilityOwner> AbilityOwnerRef;

    UPROPERTY(VisibleAnywhere, DisplayName="Ability Active?")
    bool bIsAbilityActive;

    UPROPERTY(EditAnywhere, meta=(ClampMin=1.0, ClampMax=5.0, Delta=0.1))
    double Period;

    UPROPERTY(EditAnywhere, meta=(ClampMin=75.0f, ClampMax=200.0f, Delta=5.0))
    float Amplitude;

    UPROPERTY(EditAnywhere, meta=(ClampMin=2, ClampMax=5))
    int32 Harmonic;

    UPROPERTY(EditAnywhere, meta=(ClampMin=0.0f))
    float Damage;

    UPROPERTY(EditAnywhere, meta=(ClampMin=75.0f, ClampMax=150.0f, Delta=5.0))
    float DamageRadius;

public:
    UStringAbilityComponent();

protected:
    virtual void BeginPlay() override;

public:
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    void ActivateAbility();
    void DeactivateAbility();

    UFUNCTION(BlueprintCallable)
    void UpgradeAbility();

    UFUNCTION(BlueprintCallable)
    void DegradeAbility();

    UFUNCTION(BlueprintCallable)
    void EnlargeAbility();

    UFUNCTION(BlueprintCallable)
    void ShrinkAbility();

    UFUNCTION(BlueprintCallable)
    void SpeedUpAbility();

    UFUNCTION(BlueprintCallable)
    void SlowDownAbility();

    UFUNCTION(BlueprintCallable)
    void WidenAbility();

    UFUNCTION(BlueprintCallable)
    void ConstrictAbility();

private:
    double ActivationTime;
    bool bShouldDealDamage;

    void DrawStringSegments(const FVector& PointA, const FVector& PointB, const FVector& StringNormal, const int32 NumSegments, const double SegmentDelta, const double AmplitudeModulator) const;
    void HandleDamageCycle(const double NormalizedCycleTime, const FVector& PointA, const FVector& PointB, const FVector& StringNormal);
    void DisplayDamageTelegraphs(const FVector& PointA, const FVector& PointB, const FVector& StringNormal, const double TelegraphRadius, const double PeakTime) const;
    void DealDamageAtPeaks(const FVector& PointA, const FVector& PointB, const FVector& StringNormal, const double PeakTime) const;
    static double GetNormalizedStringDisplacement(const double NormalizedPosition, const double NormalizedPhase, const int HarmonicMode);

public:
    /** Getters and Setters */

    UFUNCTION(BlueprintCallable, BlueprintPure, DisplayName="Period")
    double GetPeriod() const;

    UFUNCTION(BlueprintCallable, BlueprintPure, DisplayName="Amplitude")
    float GetAmplitude() const;

    UFUNCTION(BlueprintCallable, BlueprintPure, DisplayName="Harmonic")
    int32 GetHarmonic() const;

    UFUNCTION(BlueprintCallable, BlueprintPure, DisplayName="Damage Radius")
    float GetDamageRadius();
};
