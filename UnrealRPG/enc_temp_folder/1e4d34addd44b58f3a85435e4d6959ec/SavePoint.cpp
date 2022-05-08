// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/SavePoint.h"
#include <../Plugins/FX/Niagara/Source/Niagara/Public/NiagaraComponent.h>
#include "Components/AudioComponent.h"
#include "Player/PlayerCharacter.h"
#include "Components/SpotLightComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values
ASavePoint::ASavePoint()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	//DecolationMesh1 = CreateDefaultSubobject<UStaticMeshComponent>("DecolateMesh1");
	//DecolationMesh1->SetupAttachment(OverlapSphere);
	//DecolationMesh1->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	DecolationMesh2 = CreateDefaultSubobject<UStaticMeshComponent>("DecolateMesh2");
	DecolationMesh2->SetupAttachment(DecolationMesh);
	DecolationMesh2->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	DecolationPlane = CreateDefaultSubobject<UStaticMeshComponent>("DecolatePlane");
	DecolationPlane->SetupAttachment(DecolationMesh);
	DecolationPlane->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	FX = CreateDefaultSubobject<UNiagaraComponent>("FX");
	FX->SetupAttachment(DecolationMesh);

	Audio = CreateDefaultSubobject<UAudioComponent>("Audio");
	Audio->SetupAttachment(DecolationMesh);

	ResponPoint = CreateDefaultSubobject<USceneComponent>("ResponPoint");
	ResponPoint->SetupAttachment(DecolationMesh);


	SpotLight = CreateDefaultSubobject<USpotLightComponent>("SpotLight");
	SpotLight->SetupAttachment(DecolationMesh);
	SpotLight->SetRelativeRotation(FRotator(-90.f, 0.f, 0.f));
	SpotLight->Mobility = EComponentMobility::Stationary;
	SpotLight->Intensity = 20.f;
	SpotLight->AttenuationRadius = 265.f;
	SpotLight->InnerConeAngle = 32.f;
	SpotLight->OuterConeAngle = 50.f;

	PointLight = CreateDefaultSubobject<UPointLightComponent>("PointLight");
	PointLight->SetupAttachment(SpotLight);
	PointLight->Mobility = EComponentMobility::Stationary;
	PointLight->Intensity = 25.f;
	PointLight->AttenuationRadius = 400.f;
}

// Called when the game starts or when spawned
void ASavePoint::BeginPlay()
{
	Super::BeginPlay();
}

void ASavePoint::PlayerRangeOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
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