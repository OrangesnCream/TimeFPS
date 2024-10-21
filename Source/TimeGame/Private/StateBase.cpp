// Fill out your copyright notice in the Description page of Project Settings.


#include "StateBase.h"

UStateBase::UStateBase()
{
    // initialization logic
}

void UStateBase::EnterState()
{
    UE_LOG(LogTemp, Log, TEXT("Entering State: %s"), *StateTag.ToString());
    // Add logic for entering the state
}

void UStateBase::ExitState()
{
    UE_LOG(LogTemp, Log, TEXT("Exiting State: %s"), *StateTag.ToString());
    // Add logic for exiting the state
}

void UStateBase::UpdateState(float DeltaTime)
{
    // Add logic for updating the state each frame
}