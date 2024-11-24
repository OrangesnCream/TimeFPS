// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerBox.h"
#include "LevelEndZone.generated.h"

/**
 * 
 */
UCLASS()
class TIMEGAME_API ALevelEndZone : public ATriggerBox
{
	GENERATED_BODY()

protected:

	virtual void BeginPlay() override;

public:
	
	ALevelEndZone();

	UFUNCTION()
	void onOverlapBegin(class AActor* OverlappedActor, class AActor* OtherActor);

	UFUNCTION()
	void onOverlapEnd(class AActor* OverlappedActor, class AActor* OtherActor);
};
