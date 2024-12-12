// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_CheckPlayerDistance.h"
#include "NPC_AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include"Blueprint/AIBlueprintHelperLibrary.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "NPC.h"
UBTTask_CheckPlayerDistance::UBTTask_CheckPlayerDistance(FObjectInitializer const& ObjectInitializer)
{
	NodeName = TEXT("Check Player Distance");
}

EBTNodeResult::Type UBTTask_CheckPlayerDistance::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (auto* const Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)) {
		//get player location
		auto const PlayerLocation = Player->GetActorLocation();
		if (auto const Cont = OwnerComp.GetAIOwner()) {
			if (auto* const NPC = Cast<ANPC>(Cont->GetPawn())) {
				double distance=(PlayerLocation - NPC->GetActorLocation()).Size();
				if (distance < setDistance) {
					OwnerComp.GetBlackboardComponent()->SetValueAsBool(GetSelectedBlackboardKey(), true);
					//UE_LOG(LogTemp, Warning, TEXT("in range set to true"));
				}
				else {
					//UE_LOG(LogTemp, Warning, TEXT("in range set to false"));
					OwnerComp.GetBlackboardComponent()->SetValueAsBool(GetSelectedBlackboardKey(), false);
				}
				FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
				return EBTNodeResult::Succeeded;
				
			}
		}
		
	}

	return EBTNodeResult::Failed;
}
