// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerRun.h"

UPlayerRun::UPlayerRun()
{
    StateTag = FGameplayTag::RequestGameplayTag(FName("PlayerState.Ground.Run"));
}

void UPlayerRun::EnterState()
{
    Super::EnterState();
    if (GEngine)
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Purple, TEXT("Entering Run"));
    UE_LOG(LogTemp, Log, TEXT("Entering Run State"));
    // Add logic for entering the run state
}

void UPlayerRun::ExitState()
{
    Super::ExitState();
    if (GEngine)
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Purple, TEXT("Exiting Run"));
    UE_LOG(LogTemp, Log, TEXT("Exiting Run State"));
    // Add logic for exiting the run state
}

void UPlayerRun::UpdateState(float DeltaTime)
{
    Super::UpdateState(DeltaTime);
    // Add logic for updating the run state
}
