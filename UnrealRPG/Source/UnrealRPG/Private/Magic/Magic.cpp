// Fill out your copyright notice in the Description page of Project Settings.


#include "Magic/Magic.h"
#include <../Plugins/FX/Niagara/Source/Niagara/Public/NiagaraComponent.h>
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Sound/SoundCue.h"

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
	
	if (FXSound)
	{
		FXSound->VolumeMultiplier = 1.75f;
		UGameplayStatics::SpawnSoundAttached(
			FXSound,
			FX,
			"None",
			{ 0.f,0.f,0.f }, EAttachLocation::KeepRelativeOffset,
			true,
			0.5f,
			1.5f,
			0.f,
			(SoundAttenuation != nullptr) ? SoundAttenuation : nullptr);
	}
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

void AMagic::PlayCollisionSound()
{
	if (CollisionSound)
	{
		UGameplayStatics::SpawnSoundAttached(
			CollisionSound,
			FX,
			"None",
			{ 0.f,0.f,0.f }, EAttachLocation::KeepRelativeOffset,
			true,
			0.5f,
			1.5f,
			0.f,
			(SoundAttenuation != nullptr) ? SoundAttenuation : nullptr);
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
	//UE_LOG(LogTemp, Warning, TEXT("Magic:ActiveMagic"));
}

void AMagic::DeactiveMagic(float DeltaTime)
{
	//UE_LOG(LogTemp, Warning, TEXT("Magic:DeactiveMagic"));

	if (FXSound)
	{
		FXSound->VolumeMultiplier = UKismetMathLibrary::Lerp(FXSound->VolumeMultiplier, 0.f, DeltaTime);
	}
}