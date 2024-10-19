// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "StateBase.generated.h"

/**
 * 
 */
UCLASS()
class TIMEGAME_API UStateBase : public UObject
{
	GENERATED_BODY()
	
public:
	virtual void EnterState()
		PURE_VIRTUAL(UStateBase::EnterState, );
	virtual void ExitState()
		PURE_VIRTUAL(UStateBase::ExitState, );
	virtual void UpdateState()
		PURE_VIRTUAL(UStateBase::UpdateState, );

};
