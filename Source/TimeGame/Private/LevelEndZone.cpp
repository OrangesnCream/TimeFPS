// Fill out your copyright notice in the Description page of Project Settings.


#define print(text) if(GEngine) GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::Green, text)
#define printf(text, fstring) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::Green, Printf(TEXT(text), fstring))

#include "LevelEndZone.h"
#include "DrawDebugHelpers.h"

ALevelEndZone::ALevelEndZone() {
	OnActorBeginOverlap.AddDynamic(this, &ALevelEndZone::onOverlapBegin);
	OnActorEndOverlap.AddDynamic(this, &ALevelEndZone::onOverlapEnd);
}

void ALevelEndZone::BeginPlay() {
	Super::BeginPlay();

	DrawDebugBox(GetWorld(), GetActorLocation(), GetComponentsBoundingBox().GetExtent(), FColor::Purple, true, -1, 0, 5);
}

void ALevelEndZone::onOverlapBegin(class AActor* OverlappedActor, class AActor* OtherActor) {
	if (OtherActor && OtherActor != this) {
		print("Overlap Begin");
		// printFString("Overlapped Actor = %s", *OverlappedActor->GetName());
	}
}
void ALevelEndZone::onOverlapEnd(class AActor* OverlappedActor, class AActor* OtherActor) {
	if (OtherActor && OtherActor != this) {
		print("OVerlap Ended");
		// printFString("Overlapped Actor = %s", *OtherActor->GetName());
	}
}