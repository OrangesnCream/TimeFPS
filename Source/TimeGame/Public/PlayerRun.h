// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayerGroundState.h"
#include "PlayerRun.generated.h"

/**
 * 
 */
UCLASS()
class TIMEGAME_API UPlayerRun : public UPlayerGroundState
{
	GENERATED_BODY()
	
public:
	UPlayerRun();

	virtual void EnterState() override;
	virtual void ExitState() override;
	virtual void UpdateState(float DeltaTime) override;
};
