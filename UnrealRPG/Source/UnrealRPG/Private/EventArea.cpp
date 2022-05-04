// Fill out your copyright notice in the Description page of Project Settings.


#include "EventArea.h"
#include "Components/SphereComponent.h"
#include "Player/PlayerCharacter.h"

// Sets default values
AEventArea::AEventArea() :
	bClosePlayer(false)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	DecolationMesh = CreateDefaultSubobject<UStaticMeshComponent>("DecolateMesh1");
	DecolationMesh->SetupAttachment(RootComponent);
	DecolationMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	OverlapSphere = CreateDefaultSubobject<USphereComponent>("OverlapSphere");
	OverlapSphere->SetupAttachment(DecolationMesh);
}

// Called when the game starts or when spawned
void AEventArea::BeginPlay()
{
	Super::BeginPlay();
	
	OverlapSphere->OnComponentBeginOverlap.AddDynamic(this, &AEventArea::PlayerRangeOverlap);
	OverlapSphere->OnComponentEndOverlap.AddDynamic(this, &AEventArea::PlayerRangeEndOverlap);
}

void AEventArea::PlayerRangeOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (bClosePlayer)
	{
		return;
	}

	APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor);
	if (Player)
	{
		bClosePlayer = true;
		Player->SetEventAble(true, EventText);
	}
}

void AEventArea::PlayerRangeEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!bClosePlayer)
	{
		return;
	}

	APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor);
	if (Player)
	{
		bClosePlayer = false;
		Player->SetEventAble(false, FName());
		Player->SetButtonEventUIVisibility(false);
	}
}