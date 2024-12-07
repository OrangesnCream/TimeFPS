// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_ChasePlayer.h"
#include "NPC_AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include"Blueprint/AIBlueprintHelperLibrary.h"
UBTTask_ChasePlayer::UBTTask_ChasePlayer(FObjectInitializer const& ObjectInitializer) {
	NodeName = TEXT("chase player");

}
EBTNodeResult::Type UBTTask_ChasePlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) {
	if (auto* const cont = Cast<ANPC_AIController>(OwnerComp.GetAIOwner())) {
		auto const PlayerLocation = OwnerComp.GetBlackboardComponent()->GetValueAsVector(GetSelectedBlackboardKey());
		//if (OwnerComp.GetBlackboardComponent()->GetValueAsBool("InRange") != true) {
			//UAIBlueprintHelperLibrary::SimpleMoveToLocation(cont, PlayerLocation);
			UAIBlueprintHelperLibrary::CreateMoveToProxyObject(cont, cont->GetPawn(), PlayerLocation, nullptr, 500.0f, true);
		//}
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		return EBTNodeResult::Succeeded;

	}
	return EBTNodeResult::Failed;
}