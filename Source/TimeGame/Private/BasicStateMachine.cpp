// Fill out your copyright notice in the Description page of Project Settings.


#include "BasicStateMachine.h"

UBasicStateMachine::UBasicStateMachine() : CurrentState(nullptr)
{
}

void UBasicStateMachine::RequestState(const FGameplayTag& StateTag)
{
    if (!StateTag.IsValid())
    {
        UE_LOG(LogTemp, Warning, TEXT("Invalid Gameplay Tag requested"));
        return;
    }

    if (CurrentState)
        CurrentState->ExitState();

    UStateBase** CachedState = StateCache.Find(StateTag);
    if (CachedState)
    {
        UE_LOG(LogTemp, Log, TEXT("State found in cache: %s"), *StateTag.ToString());
        // Handle the cached state
        CurrentState = *CachedState;
    }
    else
    {
        UE_LOG(LogTemp, Log, TEXT("State not found in cache, creating new state for: %s"), *StateTag.ToString());
        // Create and add the new state to the cache
        UStateBase* NewState = NewObject<UStateBase>(this);
        NewState->StateTag = StateTag;
        AddStateToCache(StateTag, NewState);
        CurrentState = NewState;
    }

    if(CurrentState)
        CurrentState->EnterState();
}

void UBasicStateMachine::AddStateToCache(const FGameplayTag& StateTag, UStateBase* StateObject)
{
    if (StateTag.IsValid() && StateObject)
    {
        StateCache.Add(StateTag, StateObject);
        UE_LOG(LogTemp, Log, TEXT("State added to cache: %s"), *StateTag.ToString());
    }
    else
        UE_LOG(LogTemp, Warning, TEXT("Failed to add state to cache: Invalid tag or state object"));
}

UStateBase* UBasicStateMachine::GetStateFromCache(const FGameplayTag& StateTag)
{
    if (UStateBase** CachedState = StateCache.Find(StateTag))
        return *CachedState;
    return nullptr;
}