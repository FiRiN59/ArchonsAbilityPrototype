// Copyright Sergei Shavrin 2024. All Rights Reserved.

#include "StringAbilityComponent.h"

#include "Archons/Interfaces/SpanAbilityOwner.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

UStringAbilityComponent::UStringAbilityComponent()
{
    PrimaryComponentTick.bCanEverTick = true;

    bDebug = false;
    bIsAbilityActive = false;

    Period = 4.0;
    Amplitude = 100.0f;
    Harmonic = 2;

    Damage = 10.0f;
    DamageRadius = 75.0f;

    ActivationTime = 0.0;
    bShouldDealDamage = false;
}

void UStringAbilityComponent::BeginPlay()
{
    Super::BeginPlay();

    AbilityOwnerRef = Cast<ISpanAbilityOwner>(GetOwner());
    ensureAlwaysMsgf(AbilityOwnerRef.IsValid(), TEXT("Owning actor should implement %s interface"), *USpanAbilityOwner::StaticClass()->GetName());
}

void UStringAbilityComponent::ActivateAbility()
{
    if (bIsAbilityActive) { return; }

    bIsAbilityActive = true;
    ActivationTime = GetWorld()->TimeSeconds;
}

void UStringAbilityComponent::DeactivateAbility()
{
    if (!bIsAbilityActive) { return; }

    bIsAbilityActive = false;
}

void UStringAbilityComponent::UpgradeAbility()
{
    Harmonic = FMath::Clamp(Harmonic + 1, 2, 5);
}

void UStringAbilityComponent::DegradeAbility()
{
    Harmonic = FMath::Clamp(Harmonic - 1, 2, 5);
}

void UStringAbilityComponent::EnlargeAbility()
{
    DamageRadius = FMath::Clamp(DamageRadius + 25.0f, 75.0f, 150.0f);
}

void UStringAbilityComponent::ShrinkAbility()
{
    DamageRadius = FMath::Clamp(DamageRadius - 25.0f, 75.0f, 150.0f);
}

void UStringAbilityComponent::SpeedUpAbility()
{
    Period = FMath::Clamp(Period - 0.5, 1.0, 5.0);
}

void UStringAbilityComponent::SlowDownAbility()
{
    Period = FMath::Clamp(Period + 0.5, 1, 5.0);
}

void UStringAbilityComponent::WidenAbility()
{
    Amplitude = FMath::Clamp(Amplitude + 25.0f, 75.0f, 200.0f);
}

void UStringAbilityComponent::ConstrictAbility()
{
    Amplitude = FMath::Clamp(Amplitude - 25.0f, 75.0f, 200.0f);
}

void UStringAbilityComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (!bDebug || !bIsAbilityActive || !AbilityOwnerRef.IsValid()) { return; }

    FVector PointA, PointB;
    if (!AbilityOwnerRef->GetAbilitySpan(PointA, PointB)) { return; }

    // Calculate the current position in the oscillation cycle
    const double ElapsedTime = GetWorld()->TimeSeconds - ActivationTime;
    const double CycleTime = FMath::Wrap(ElapsedTime, 0.0, Period);
    const double NormalizedCycleTime = CycleTime / Period;

    const FVector StringNormal = FVector::CrossProduct(PointB - PointA, FVector::ZAxisVector).GetSafeNormal();

    constexpr int32 NumSegments = 20;
    constexpr double SegmentDelta = 1.0 / static_cast<double>(NumSegments - 1);

    // Modulate the wave amplitude based on the position in the cycle
    const double AmplitudeModulator = FMath::Cos(NormalizedCycleTime * UE_DOUBLE_TWO_PI);
    DrawStringSegments(PointA, PointB, StringNormal, NumSegments, SegmentDelta, AmplitudeModulator);

    HandleDamageCycle(NormalizedCycleTime, PointA, PointB, StringNormal);
}

void UStringAbilityComponent::DrawStringSegments(const FVector& PointA, const FVector& PointB, const FVector& StringNormal, int32 NumSegments, double SegmentDelta, double AmplitudeModulator) const
{
    for (int32 Segment = 0; Segment < NumSegments; ++Segment)
    {
        const double OffsetNorm = SegmentDelta * static_cast<double>(Segment);
        const FVector SegmentPosition = UKismetMathLibrary::VLerp(PointA, PointB, OffsetNorm);
        const double WaveValue = FMath::Sin(OffsetNorm * UE_DOUBLE_PI * static_cast<double>(Harmonic)) * Amplitude * AmplitudeModulator;

        DrawDebugPoint(GetWorld(), SegmentPosition + StringNormal * WaveValue, 4, FColor::Blue);
    }
}

// Handle displaying telegraphs and dealing damage based on the current cycle time
void UStringAbilityComponent::HandleDamageCycle(const double NormalizedCycleTime, const FVector& PointA, const FVector& PointB, const FVector& StringNormal)
{
    // The cycle starts and ends on the same peak value, that's why there are 2 values for the range comparisons to work properly
    constexpr double PeakTime1_0 = 0.0;
    constexpr double ZeroAmplitudeTime1 = 0.25;
    constexpr double PeakTime2 = 0.5;
    constexpr double ZeroAmplitudeTime2 = 0.75;
    constexpr double PeakTime1_1 = 1.0;

    // Display telegraph effects
    if (FMath::IsWithinInclusive(NormalizedCycleTime, ZeroAmplitudeTime1, PeakTime2))
    {
        bShouldDealDamage = true;
        const double TelegraphRadius = FMath::Lerp(0.0, DamageRadius, (NormalizedCycleTime - ZeroAmplitudeTime1) * 4.0);
        DisplayDamageTelegraphs(PointA, PointB, StringNormal, TelegraphRadius, PeakTime2);
    }
    else if (FMath::IsWithinInclusive(NormalizedCycleTime, ZeroAmplitudeTime2, PeakTime1_1))
    {
        bShouldDealDamage = true;
        const double TelegraphRadius = FMath::Lerp(0.0, DamageRadius, (NormalizedCycleTime - ZeroAmplitudeTime2) * 4.0);
        DisplayDamageTelegraphs(PointA, PointB, StringNormal, TelegraphRadius, PeakTime1_1);
    }
    // Apply radial damage at each peak after the telegraph phase ends
    else if (bShouldDealDamage && FMath::IsWithinInclusive(NormalizedCycleTime, PeakTime1_0, ZeroAmplitudeTime1))
    {
        bShouldDealDamage = false;
        DealDamageAtPeaks(PointA, PointB, StringNormal, PeakTime1_0);
    }
    else if (bShouldDealDamage && FMath::IsWithinInclusive(NormalizedCycleTime, PeakTime2, ZeroAmplitudeTime2))
    {
        bShouldDealDamage = false;
        DealDamageAtPeaks(PointA, PointB, StringNormal, PeakTime2);
    }
}

void UStringAbilityComponent::DisplayDamageTelegraphs(const FVector& PointA, const FVector& PointB, const FVector& StringNormal, const double TelegraphRadius, const double PeakTime) const
{
    for (int32 PeakNumber = 0; PeakNumber < Harmonic; ++PeakNumber)
    {
        const double PeakOffsetNorm = static_cast<double>(2 * PeakNumber + 1) / static_cast<double>(2 * Harmonic);
        const double PeakDisplacement = GetNormalizedStringDisplacement(PeakOffsetNorm, PeakTime, Harmonic) * Amplitude;
        const FVector PeakPosition = UKismetMathLibrary::VLerp(PointA, PointB, PeakOffsetNorm) + StringNormal * PeakDisplacement;

        DrawDebugCircle(GetWorld(), PeakPosition, TelegraphRadius, 32, FColor::Blue, false, -1.0f, 0, 2, FVector::XAxisVector, FVector::YAxisVector, false);
    }
}

void UStringAbilityComponent::DealDamageAtPeaks(const FVector& PointA, const FVector& PointB, const FVector& StringNormal, const double PeakTime) const
{
    for (int32 PeakNumber = 0; PeakNumber < Harmonic; ++PeakNumber)
    {
        const double PeakOffsetNorm = static_cast<double>(2 * PeakNumber + 1) / static_cast<double>(2 * Harmonic);
        const double PeakDisplacement = GetNormalizedStringDisplacement(PeakOffsetNorm, PeakTime, Harmonic) * Amplitude;
        const FVector PeakPosition = UKismetMathLibrary::VLerp(PointA, PointB, PeakOffsetNorm) + StringNormal * PeakDisplacement;

        DrawDebugCircle(GetWorld(), PeakPosition, DamageRadius, 32, FColor::Red, false, 0.25f, 0, 2, FVector::XAxisVector, FVector::YAxisVector, false);

        TArray<AActor*> IgnoreActors;
        AbilityOwnerRef->GetIgnoreDamageActors(IgnoreActors);
        const FVector DamageOrigin = PeakPosition + FVector::ZAxisVector * 34.0;

        UGameplayStatics::ApplyRadialDamage(this, Damage, DamageOrigin, DamageRadius, UDamageType::StaticClass(), IgnoreActors, GetOwner(), GetOwner()->GetInstigatorController(), true);
    }
}

// Calculate wave displacement based on the harmonic mode and time progression
double UStringAbilityComponent::GetNormalizedStringDisplacement(const double NormalizedPosition, const double NormalizedPhase, const int HarmonicMode)
{
    return FMath::Sin(NormalizedPosition * UE_DOUBLE_PI * static_cast<double>(HarmonicMode)) * FMath::Cos(NormalizedPhase * UE_DOUBLE_TWO_PI);
}

double UStringAbilityComponent::GetPeriod() const
{
    return Period;
}

float UStringAbilityComponent::GetAmplitude() const
{
    return Amplitude;
}

int32 UStringAbilityComponent::GetHarmonic() const
{
    return Harmonic;
}

float UStringAbilityComponent::GetDamageRadius()
{
    return DamageRadius;
}
