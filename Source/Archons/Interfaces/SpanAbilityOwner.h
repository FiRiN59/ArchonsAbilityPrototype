// Copyright Sergei Shavrin 2024. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SpanAbilityOwner.generated.h"

UINTERFACE()
class USpanAbilityOwner : public UInterface
{
    GENERATED_BODY()
};

class ARCHONS_API ISpanAbilityOwner
{
    GENERATED_BODY()

public:
    virtual bool GetAbilitySpan(FVector& OutPointA, FVector& OutPointB) const { return false; };
    virtual void GetIgnoreDamageActors(TArray<AActor*>& OutActors) const { };
};
