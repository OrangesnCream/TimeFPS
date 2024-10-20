// Fill out your copyright notice in the Description page of Project Settings.


#include "TimeManager.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Engine/StaticMeshActor.h"

// Sets default values
ATimeManager::ATimeManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ATimeManager::BeginPlay()
{
	Super::BeginPlay();
    GetWorld()->AddOnActorSpawnedHandler(FOnActorSpawned::FDelegate::CreateUObject(this, &ATimeManager::HandleActorSpawned));

    for (TActorIterator<AActor> It(GetWorld()); It; ++It)
    {
        AActor* Actor = *It;

        ActorsToAffect.Add(Actor);

    }
    for (TActorIterator<AStaticMeshActor> It(GetWorld()); It; ++It)
    {
        //I added this because it wasn't adding the cube -_-
        AStaticMeshActor* ActorS = *It;
        //custom_time_dilation
            UE_LOG(LogTemp, Warning, TEXT("Found Static Mesh Actor: "));
            StaticActorsToAffect.Add(ActorS);

    }

}

// Called every frame
void ATimeManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
   
}

void ATimeManager::HandleActorSpawned(AActor* SpawnedActor)
{
   // if (SpawnedActor && SpawnedActor->IsA(/*add something here IDK*/ ))  // Replace with your actor filtering logic
   // {
        ActorsToAffect.Add(SpawnedActor);
        if (abilityActive) {
            SpawnedActor->CustomTimeDilation = globalTime;
        }
        SpawnedActor->OnDestroyed.AddDynamic(this, &ATimeManager::HandleActorDestroyed);

   // }
}

/*
* this broke the player controls
void ATimeManager::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
    // Bind input actions to functions
    PlayerInputComponent->BindAction("Ability", IE_Pressed, this, &ATimeManager::GlobalActorSlowdown);
}
*/
void ATimeManager::GlobalActorSlowdown(){
    if (abilityActive) {
        for (AActor* Actor : ActorsToAffect)
        {
            Actor->CustomTimeDilation = 1.0;   
        }
        abilityActive = false;
        return;
    }

    for (AActor* Actor : ActorsToAffect)
    {
        Actor->CustomTimeDilation = globalTime;
        abilityActive = true;
    }
}

void ATimeManager::HandleActorDestroyed(AActor* DestroyedActor)
{
    // Remove the destroyed actor from the list
    if (ActorsToAffect.Contains(DestroyedActor))
    {
        ActorsToAffect.Remove(DestroyedActor);
    }
}
