// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/LionKnight/LionKnight.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Magic/ProjectileMagic.h"

ALionKnight::ALionKnight():
	ProjectileDamage(50.f)
{

}

void ALionKnight::PlayAttackMontage()
{
	if (AnimInstance)
	{
		if (AdvancedAttackMontage.IsValidIndex(AttackIndex) && AdvancedAttackMontage[AttackIndex].AttackMontage)
		{
			AnimInstance->Montage_Play(AdvancedAttackMontage[AttackIndex].AttackMontage);
			LastAttackIndex = AttackIndex;

			UE_LOG(LogTemp, Warning, TEXT("Play Attack Montage"));
		}

		AttackIndex++;
		if (AdvancedAttackMontage.Num() == AttackIndex)
		{
			AttackIndex = 0;
		}
	}
}

void ALionKnight::BeginPlay()
{
	Super::BeginPlay();

	AD = 30.f;
	HP = 2000.f;

	ChangeEnemySize(EEnemySize::EES_Large);

	GetCharacterMovement()->MaxWalkSpeed = MaximumSprintSpeed;
}

void ALionKnight::UseMagic()
{
	if (ProjectileMagic)
	{
		const FVector MagicStartLoc{ GetActorLocation() + (GetActorForwardVector() * 150.f) };

		AProjectileMagic* SpawnMagic = GetWorld()->SpawnActor<AProjectileMagic>(ProjectileMagic, MagicStartLoc, GetActorRotation());
		SpawnMagic->InitMagic(this, ProjectileDamage);
	}
}