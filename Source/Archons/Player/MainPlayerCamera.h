// Copyright Sergei Shavrin 2024. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MainPlayerCamera.generated.h"

class USpringArmComponent;
class UCameraComponent;

UCLASS()
class ARCHONS_API AMainPlayerCamera : public APawn
{
    GENERATED_BODY()

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    USceneComponent* RootSceneComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    USpringArmComponent* SpringArmComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UCameraComponent* CameraComponent;

public:
    AMainPlayerCamera();

    FRotator GetCameraRotation() const;
};
