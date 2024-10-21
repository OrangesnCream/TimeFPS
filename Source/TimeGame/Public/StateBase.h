// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameplayTagsClasses.h"
#include "StateBase.generated.h"

/**
 * 
 */
UCLASS()
class TIMEGAME_API UStateBase : public UObject
{
	GENERATED_BODY()
	
public:
	UStateBase();

	virtual void EnterState();
	virtual void ExitState();
	virtual void UpdateState(float DeltaTime);

	FGameplayTag StateTag;
};
