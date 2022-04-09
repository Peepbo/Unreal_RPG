// Fill out your copyright notice in the Description page of Project Settings.


#include "Magic/Magic.h"
#include <../Plugins/FX/Niagara/Source/Niagara/Public/NiagaraComponent.h>

// Sets default values
AMagic::AMagic():
	MagicDamage(0.f),
	FXSize(1.f),
	FXSpawnRate(1.f)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	FX = CreateDefaultSubobject<UNiagaraComponent>(TEXT("FX"));
	FX->SetupAttachment(RootComponent);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(FX);
}

// Called when the game starts or when spawned
void AMagic::BeginPlay()
{
	Super::BeginPlay();
	
}

void AMagic::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (FX && Mesh) 
	{
		Mesh->SetRelativeScale3D({ FXSize,FXSize ,FXSize });

		FX->SetFloatParameter(FireParam, FXSpawnRate);
		FX->SetFloatParameter(SizeParam, FXSize);
	}
}

// Called every frame
void AMagic::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMagic::InitMagic(APawn* CastOwner, float Damage)
{
	if (CastOwner)
	{
		MagicOwner = CastOwner;
	}
	MagicDamage = Damage;
}

void AMagic::ActiveMagic(float DeltaTime)
{
	UE_LOG(LogTemp, Warning, TEXT("Magic:ActiveMagic"));
}

void AMagic::DeactiveMagic(float DeltaTime)
{
	UE_LOG(LogTemp, Warning, TEXT("Magic:DeactiveMagic"));
}