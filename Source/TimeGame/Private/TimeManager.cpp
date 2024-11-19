// Fill out your copyright notice in the Description page of Project Settings.


#include "TimeManager.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Engine/StaticMeshActor.h"
//#include "../TimeGameProjectile.h"
#include "../TimeGameCharacter.h"

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
        Actor->OnDestroyed.AddDynamic(this, &ATimeManager::HandleActorDestroyed);
        ActorsToAffect.Add(Actor);

    }
    for (TActorIterator<AStaticMeshActor> It(GetWorld()); It; ++It)
    {
        //I added this because it wasn't adding the cube -_-
        AStaticMeshActor* ActorS = *It;
        ActorS->OnDestroyed.AddDynamic(this, &ATimeManager::HandleActorDestroyed);
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
            if (/*SpawnedActor->IsA(ATimeGameProjectile::StaticClass())*/ false) {
                SpawnedActor->CustomTimeDilation = bulletTime;
            }
            else if (SpawnedActor->IsA(ATimeGameCharacter::StaticClass())) {
                SpawnedActor->CustomTimeDilation = playerTime;
            }
            else {
                SpawnedActor->CustomTimeDilation = globalTime;
            }
        }
        SpawnedActor->OnDestroyed.AddDynamic(this, &ATimeManager::HandleActorDestroyed);

   // }
}


void ATimeManager::GlobalActorSlowdown()
{
    if (abilityActive) {//resets when Ability is used again
        for (AActor* Actor : ActorsToAffect)
        {
            Actor->CustomTimeDilation = 1.0;  

        }


        //loop for static meshes
        for (AStaticMeshActor* Actor : StaticActorsToAffect)
        {
            Actor->CustomTimeDilation = 1.0;

            if (Actor->GetStaticMeshComponent())
            {

                UStaticMeshComponent* MeshComponent = Actor->GetStaticMeshComponent();
                if (MeshComponent->Mobility != EComponentMobility::Movable) {//check if static mesh is actually a thing we can move
                    continue;
                }


                FVector LinearVelocity = MeshComponent->GetPhysicsLinearVelocity();
                MeshComponent->SetPhysicsLinearVelocity(LinearVelocity/globalTime);

                FVector AngularVelocity = MeshComponent->GetPhysicsAngularVelocityInDegrees();
                MeshComponent->SetPhysicsAngularVelocityInDegrees(AngularVelocity / globalTime);
            }
        }

        abilityActive = false;
        return;
    }


    abilityActive = true;
    for (AActor* Actor : ActorsToAffect)
    {//loop that applies slow down
        if (/*Actor->IsA(ATimeGameProjectile::StaticClass())*/false) {
            Actor->CustomTimeDilation = bulletTime;
        }
        else if (Actor->IsA(ATimeGameCharacter::StaticClass())) {
            Actor->CustomTimeDilation = playerTime;
        }
        else {
            Actor->CustomTimeDilation = globalTime;
        }
        
    }

    //seperate loop for staticmesh actors in scene
    for (AStaticMeshActor* Actor : StaticActorsToAffect)
    {
        Actor->CustomTimeDilation = globalTime;

        if (Actor->GetStaticMeshComponent())
        {
           
            UStaticMeshComponent* MeshComponent = Actor->GetStaticMeshComponent();
            if (MeshComponent->Mobility != EComponentMobility::Movable) {//check if static mesh is actually a thing we can move
                continue;
            }
            FVector LinearVelocity = MeshComponent->GetPhysicsLinearVelocity();
            MeshComponent->SetPhysicsLinearVelocity(LinearVelocity * globalTime);

            FVector AngularVelocity = MeshComponent->GetPhysicsAngularVelocityInDegrees();
            MeshComponent->SetPhysicsAngularVelocityInDegrees(AngularVelocity * globalTime);
        }
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
