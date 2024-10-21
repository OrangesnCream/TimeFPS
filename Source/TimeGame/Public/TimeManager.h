// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TimeManager.generated.h"

UCLASS()
class TIMEGAME_API ATimeManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATimeManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	void GlobalActorSlowdown();
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	

private:
	//add values here later for preset changes
	//these values will be used to set the 
	float globalTime;
	float playerTime;
	float enemyTime;
	float bulletTime;

};
