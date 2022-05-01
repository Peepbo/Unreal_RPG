// Fill out your copyright notice in the Description page of Project Settings.


#include "EventArea.h"

// Sets default values
AEventArea::AEventArea()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AEventArea::BeginPlay()
{
	Super::BeginPlay();
	
}

void AEventArea::PlayerRangeOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
}

void AEventArea::PlayerRangeEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
}

// Called every frame
void AEventArea::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

