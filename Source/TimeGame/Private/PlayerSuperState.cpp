// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerSuperState.h"

UPlayerSuperState::UPlayerSuperState()
{
}

void UPlayerSuperState::EnterState()
{
    Super::EnterState();
    // Add logic for entering the player super state
}

void UPlayerSuperState::ExitState()
{
    Super::ExitState();
    // Add logic for exiting the player super state
}

void UPlayerSuperState::UpdateState(float DeltaTime)
{
    Super::UpdateState(DeltaTime);
    // Add logic for updating the player super state
}