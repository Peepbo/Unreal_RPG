// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/SavePoint.h"
#include <../Plugins/FX/Niagara/Source/Niagara/Public/NiagaraComponent.h>
#include "Components/SphereComponent.h"
#include "Components/AudioComponent.h"
#include "Player/PlayerCharacter.h"
#include "Components/SpotLightComponent.h"

#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values
ASavePoint::ASavePoint():
	bClosePlayer(false)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	DecolationMesh1 = CreateDefaultSubobject<UStaticMeshComponent>("DecolateMesh1");
	DecolationMesh1->SetupAttachment(RootComponent);
	DecolationMesh1->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	DecolationMesh2 = CreateDefaultSubobject<UStaticMeshComponent>("DecolateMesh2");
	DecolationMesh2->SetupAttachment(DecolationMesh1);
	DecolationMesh2->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	DecolationPlane = CreateDefaultSubobject<UStaticMeshComponent>("DecolatePlane");
	DecolationPlane->SetupAttachment(DecolationMesh1);
	DecolationPlane->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	FX = CreateDefaultSubobject<UNiagaraComponent>("FX");
	FX->SetupAttachment(DecolationMesh1);

	Audio = CreateDefaultSubobject<UAudioComponent>("Audio");
	Audio->SetupAttachment(DecolationMesh1);

	OverlapSphere = CreateDefaultSubobject<USphereComponent>("OverlapSphere");
	OverlapSphere->SetupAttachment(DecolationMesh1);
	OverlapSphere->SetSphereRadius(250.f);

	ResponPoint = CreateDefaultSubobject<USceneComponent>("ResponPoint");
	ResponPoint->SetupAttachment(DecolationMesh1);


	SpotLight = CreateDefaultSubobject<USpotLightComponent>("SpotLight");
	SpotLight->SetupAttachment(DecolationMesh1);
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

void ASavePoint::ChangeOverlapSetting(APlayerCharacter* Player)
{
	bClosePlayer = true;

	Player->SetEventAble(true);
	Player->SetCloseSavePoint(this);
}

// Called when the game starts or when spawned
void ASavePoint::BeginPlay()
{
	Super::BeginPlay();
	
	OverlapSphere->OnComponentBeginOverlap.AddDynamic(this, &ASavePoint::PlayerRangeOverlap);
	OverlapSphere->OnComponentEndOverlap.AddDynamic(this, &ASavePoint::PlayerRangeEndOverlap);
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
		UE_LOG(LogTemp, Warning, TEXT("You can Rest"));
		bClosePlayer = true;

		Player->SetEventAble(true);
		Player->SetCloseSavePoint(this);
	}
}

void ASavePoint::PlayerRangeEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!bClosePlayer)
	{
		return;
	}

	APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor);
	if (Player)
	{
		UE_LOG(LogTemp, Warning, TEXT("You can't Rest"));
		bClosePlayer = false;

		Player->SetEventAble(false);
	}
}

// Called every frame
void ASavePoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

