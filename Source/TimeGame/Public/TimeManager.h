// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"

#include "TimeManager.generated.h"

UCLASS()
class TIMEGAME_API ATimeManager : public APawn
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATimeManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	TArray<AActor*> ActorsToAffect;
	TArray<AStaticMeshActor*> StaticActorsToAffect;
	void HandleActorSpawned(AActor* SpawnedActor);

	UFUNCTION()
	void HandleActorDestroyed(AActor* DestroyedActor);
	
	
public:	
	void GlobalActorSlowdown();
	//virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	

private:
	//add values here later for preset changes
	//these values will be used to set the 
	float globalTime=0.5;
	float playerTime=1.0;
	float enemyTime=0.3;
	float bulletTime=0.4;
	bool  abilityActive=false;
};
