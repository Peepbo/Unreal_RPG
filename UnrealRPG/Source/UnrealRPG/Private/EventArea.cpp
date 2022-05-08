// Fill out your copyright notice in the Description page of Project Settings.


#include "EventArea.h"
#include "Components/SphereComponent.h"
#include "Player/PlayerCharacter.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
AEventArea::AEventArea() :
	bClosePlayer(false),
	Distance(0.f)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	DecolationMesh = CreateDefaultSubobject<UStaticMeshComponent>("DecolateMesh1");
	DecolationMesh->SetupAttachment(RootComponent);
	DecolationMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	OverlapSphere = CreateDefaultSubobject<USphereComponent>("OverlapSphere");
	OverlapSphere->SetupAttachment(DecolationMesh);
}

void AEventArea::ActiveAutoArrange(float Dist)
{
	Distance = Dist;
	InitEventLocationAndRotation();

	MoveTargetEventLocation();
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
		PlayerCharacter = Player;
		bClosePlayer = true;
		PlayerCharacter->SetEventAble(true, this);
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
		Player->SetEventAble(false, nullptr);
		Player->SetButtonEventUIVisibility(false);
	}
}

bool AEventArea::MoveTargetEventLocation()
{
	PlayerCharacter->SetActorLocation(EventLocation);
	PlayerCharacter->SetActorRotation(EventRotator);

	return true;
}

void AEventArea::InitEventLocationAndRotation()
{
	const FVector RestToPlayer{ PlayerCharacter->GetActorLocation() - GetActorLocation() };
	const FVector RestToPlayerIgnoreZ{ RestToPlayer.GetSafeNormal2D() };

	EventLocation = GetActorLocation() + (RestToPlayerIgnoreZ * Distance);
	EventLocation.Z = PlayerCharacter->GetActorLocation().Z;

	// 최종 회전 값을 구한다.
	EventRotator.Yaw = UKismetMathLibrary::FindLookAtRotation(PlayerCharacter->GetActorLocation(), GetActorLocation()).Yaw;
}