// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_FindPathPoint.generated.h"

/**
 * 
 */
UCLASS()
class TIMEGAME_API UBTTask_FindPathPoint : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
public:
	explicit UBTTask_FindPathPoint(FObjectInitializer const& ObjectInitializer);
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)override;
private:
	UPROPERTY(EditAnywhere,Category="Blackboard", meta=(AllowPrivateAccess="true"))
	FBlackboardKeySelector PatrolPathVectorKey;

};
