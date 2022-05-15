// Fill out your copyright notice in the Description page of Project Settings.


#include "Magic/LaserBeam.h"
#include <../Plugins/FX/Niagara/Source/Niagara/Public/NiagaraComponent.h>
#include "Kismet/KismetSystemLibrary.h"
#include "MeleeCharacter.h"
#include "TimerManager.h"

ALaserBeam::ALaserBeam():
	bDrawDebugTrace(false)
{
	StartPointFX = CreateDefaultSubobject<UNiagaraComponent>(TEXT("StartPoint FX"));
	StartPointFX->SetupAttachment(FX);

	EndPointFX = CreateDefaultSubobject<UNiagaraComponent>(TEXT("EndPoint FX"));
	StartPointFX->SetupAttachment(FX);

	// Static mesh가 필요없다. (필요하면 true)
	SetMeshVisiblilty(false);
}

void ALaserBeam::BeginPlay()
{
	Super::BeginPlay();

	GetWorldTimerManager().SetTimer(
		DestroyTimer,
		FTimerDelegate::CreateLambda([&]
			{
				Destroy();
			}),
		NiagaraLifeTime,
		false);
}

void ALaserBeam::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TraceLifeTime -= DeltaTime;

	FHitResult Hit;
	UKismetSystemLibrary::SphereTraceSingle(
		this,
		GetActorLocation(),
		GetActorLocation() + GetActorForwardVector() * TraceMaximumDistance,
		TraceSphereRadius,
		ETraceTypeQuery::TraceTypeQuery1,
		false,
		{ MagicOwner },
		bDrawDebugTrace ? EDrawDebugTrace::ForOneFrame : EDrawDebugTrace::None,
		Hit,
		true);

	if (Hit.bBlockingHit)
	{
		EndPointFX->SetWorldLocation(Hit.Location);

		if (Hit.GetActor())
		{
			AMeleeCharacter* MeleeCharacter = Cast<AMeleeCharacter>(Hit.GetActor());
			const bool bCastableMeleeCharacter{ Hit.GetActor() != nullptr && MeleeCharacter != nullptr };
			const bool bApplyDamageable{ bCastableMeleeCharacter && MeleeCharacter->GetCombatState() != ECombatState::ECS_Roll && MeleeCharacter->DamageableState() };

			if (Hit.GetActor() != nullptr && (bApplyDamageable || !bCastableMeleeCharacter))
			{
				EndPointFX->SetWorldLocation(Hit.Location);
			}

			if (!bStop && TraceLifeTime > 0.f)
			{
				if (bCastableMeleeCharacter && MeleeCharacter->GetCombatState() != ECombatState::ECS_Roll && MeleeCharacter->DamageableState())
				{
					AMeleeCharacter* Character = Cast<AMeleeCharacter>(Hit.GetActor());
					Character->CustomApplyDamage(Character, MagicDamage, MagicOwner, EAttackType::EAT_Strong);
					bStop = true;
				}
			}
		}
	}
}