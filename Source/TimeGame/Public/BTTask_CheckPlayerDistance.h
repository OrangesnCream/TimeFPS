// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_CheckPlayerDistance.generated.h"

/**
 * 
 */
UCLASS()
class TIMEGAME_API UBTTask_CheckPlayerDistance : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
public:
	explicit UBTTask_CheckPlayerDistance(FObjectInitializer const& ObjectInitializer);
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)override;
private:
	//UPROPERTY(EditAnywhere, Category = "Blackboard", meta = (AllowPrivateAccess = "true"))
	//FBlackboardKeySelector InRange;
	UPROPERTY(EditAnywhere, Category = "Blackboard", meta = (AllowPrivateAccess = "true"))
	double setDistance=300.0;
};
