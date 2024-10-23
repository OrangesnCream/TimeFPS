// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerIdle.h"

UPlayerIdle::UPlayerIdle()
{
    StateTag = FGameplayTag::RequestGameplayTag(FName("PlayerState.Ground.Idle"));
}

void UPlayerIdle::EnterState()
{
    Super::EnterState();
    if (GEngine)
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("Entering Idle"));
    UE_LOG(LogTemp, Log, TEXT("Entering Idle State"));
    // Add logic for entering the idle state
}

void UPlayerIdle::ExitState()
{
    Super::ExitState();
    if (GEngine)
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("Exiting Idle"));
    UE_LOG(LogTemp, Log, TEXT("Exiting Idle State"));
    // Add logic for exiting the idle state
}

void UPlayerIdle::UpdateState(float DeltaTime)
{
    Super::UpdateState(DeltaTime);
    // Add logic for updating the idle state
}
