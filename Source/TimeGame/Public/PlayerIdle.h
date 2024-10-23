// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayerGroundState.h"
#include "PlayerIdle.generated.h"

/**
 * 
 */
UCLASS()
class TIMEGAME_API UPlayerIdle : public UPlayerGroundState
{
	GENERATED_BODY()

public:
    UPlayerIdle();

    virtual void EnterState() override;
    virtual void ExitState() override;
    virtual void UpdateState(float DeltaTime) override;
};
