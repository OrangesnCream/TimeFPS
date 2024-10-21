// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayerSuperState.h"
#include "PlayerGroundState.generated.h"

/**
 * 
 */
UCLASS()
class TIMEGAME_API UPlayerGroundState : public UPlayerSuperState
{
	GENERATED_BODY()
	
public:
	UPlayerGroundState();

	virtual void EnterState() override;
	virtual void ExitState() override;
	virtual void UpdateState(float DeltaTime) override;
};
