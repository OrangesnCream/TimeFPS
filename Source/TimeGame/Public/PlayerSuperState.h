// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StateBase.h"
#include "PlayerSuperState.generated.h"

/**
 * 
 */
UCLASS()
class TIMEGAME_API UPlayerSuperState : public UStateBase
{
	GENERATED_BODY()
	
public:
	UPlayerSuperState();

	virtual void EnterState() override;
	virtual void ExitState() override;
	virtual void UpdateState(float DeltaTime) override;
};
