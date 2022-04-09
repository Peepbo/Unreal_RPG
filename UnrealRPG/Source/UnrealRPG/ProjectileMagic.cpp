// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileMagic.h"
#include "Kismet/GameplayStatics.h"
#include "MeleeCharacter.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "NiagaraFunctionLibrary.h"
#include <../Plugins/FX/Niagara/Source/Niagara/Public/NiagaraComponent.h>

AProjectileMagic::AProjectileMagic():
	bUseLerp(false),
	bStop(false),
	bAutoFadeOut(false),
	FXSpeed(1.f),
	MaximumFXSpeed(15.f),
	SpawnRateFadeOutSpeed(1.f)
{

}

void AProjectileMagic::BeginPlay()
{
	Super::BeginPlay();
}

void AProjectileMagic::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (MagicOwner == nullptr)
	{
		return;
	}

	if (!bStop)
	{
		ActiveMagic(DeltaTime);
	}
	else
	{
		DeactiveMagic(DeltaTime);
	}
}

void AProjectileMagic::ActiveMagic(float DeltaTime)
{
	AddActorLocalOffset({ FXSpeed,0.f,0.f });
	FXSpeed = UKismetMathLibrary::Lerp(FXSpeed, MaximumFXSpeed, DeltaTime * 2.f);

	FHitResult HitResult;
	UKismetSystemLibrary::SphereTraceSingle(
		this,
		GetActorLocation(), GetActorLocation(),
		100.f,
		ETraceTypeQuery::TraceTypeQuery1,
		false,
		{ MagicOwner },
		EDrawDebugTrace::None,
		HitResult,
		true);

	if (HitResult.bBlockingHit)
	{
		bool bPlayFX{ true };
		if (HitResult.GetActor() != MagicOwner)
		{
			AMeleeCharacter* MeleeCharacter = Cast<AMeleeCharacter>(HitResult.GetActor());

			if (MeleeCharacter)
			{
				if (MeleeCharacter->GetCombatState() != ECombatState::ECS_Roll)
				{
					UGameplayStatics::ApplyDamage(
						MeleeCharacter,
						MagicDamage,
						MagicOwner->GetController(),
						MagicOwner,
						UDamageType::StaticClass());
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("check MeleeCharacter, but state 'Roll'"));
					bPlayFX = false;
				}
			}


			if (bPlayFX)
			{
				if (EndFX_particle)
				{
					UGameplayStatics::SpawnEmitterAtLocation(
						GetWorld(),
						EndFX_particle,
						HitResult.Location);
				}

				if (EndFX_niagara)
				{
					UNiagaraFunctionLibrary::SpawnSystemAtLocation(
						GetWorld(),
						EndFX_niagara,
						HitResult.Location,
						GetActorRotation());
				}

				Mesh->SetVisibility(false);
				FXSpawnRate = 0.5f;

				bStop = true;
			}
		}
	}
}

void AProjectileMagic::DeactiveMagic(float DeltaTime)
{
	//UE_LOG(LogTemp, Warning, TEXT("%f"), FXSpawnRate);
	const bool bDestroy{ !bAutoFadeOut || UKismetMathLibrary::NearlyEqual_FloatFloat(FXSpawnRate, 0.f) };
	if (bDestroy)
	{
		Destroy();
	}

	FXSpawnRate = UKismetMathLibrary::Lerp(FXSpawnRate, 0.f, DeltaTime * SpawnRateFadeOutSpeed);
	FX->SetFloatParameter(FireParam, FXSpawnRate);
}
