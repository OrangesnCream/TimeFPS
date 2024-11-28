// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_ChangeSpeed.h"
#include "NPC.h"
#include "AIController.h"
#include "GameFramework/CharacterMovementComponent.h"

UBTService_ChangeSpeed::UBTService_ChangeSpeed()
{
	bNotifyBecomeRelevant = true;
	NodeName = TEXT("Change Speed");
}

void UBTService_ChangeSpeed::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (auto const Cont = OwnerComp.GetAIOwner()) {
		if (auto* const NPC = Cast<ANPC>(Cont->GetPawn())) {
		
			NPC->GetCharacterMovement()->MaxWalkSpeed = speed;
		}
	}
}
