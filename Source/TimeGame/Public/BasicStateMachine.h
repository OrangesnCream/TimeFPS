// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameplayTagsClasses.h"
#include "Delegates/Delegate.h"
#include "StateBase.h"
#include "BasicStateMachine.generated.h"

/**
 * 
 */
UCLASS()
class TIMEGAME_API UBasicStateMachine : public UObject
{
	GENERATED_BODY()

public:
    UBasicStateMachine();

    void RequestState(const FGameplayTag& StateTag);
    void AddStateToCache(const FGameplayTag& StateTag, UStateBase* StateObject);
    UStateBase* GetStateFromCache(const FGameplayTag& StateTag);

private:
    // Possible states (could be defined dynamically or via configuration)
    TMap<FGameplayTag, UStateBase*> StateCache;
    UStateBase* CurrentState;

};