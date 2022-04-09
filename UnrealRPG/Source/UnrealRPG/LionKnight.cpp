// Fill out your copyright notice in the Description page of Project Settings.


#include "LionKnight.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "ProjectileMagic.h"

ALionKnight::ALionKnight():
	ProjectileDamage(50.f)
{

}

void ALionKnight::PlayAttackMontage()
{
	if (AnimInstance)
	{
		if (AttackMontage.IsValidIndex(AttackIndex) && AttackMontage[AttackIndex])
		{
			AnimInstance->Montage_Play(AttackMontage[AttackIndex]);
			LastAttackIndex = AttackIndex;

			UE_LOG(LogTemp, Warning, TEXT("Play Attack Montage"));
		}

		AttackIndex++;
		if (AttackMontage.Num() == AttackIndex)
		{
			AttackIndex = 0;
		}
	}
}

void ALionKnight::BeginPlay()
{
	Super::BeginPlay();

	ChangeEnemySize(EEnemySize::EES_Large);

	GetCharacterMovement()->MaxWalkSpeed = MaximumSprintSpeed;

	//if (ProjectileMagic)
	//{
	//	AProjectileMagic* ProjectMagic = Cast<AProjectileMagic>(ProjectileMagic);
	//	if (ProjectMagic)
	//	{
	//		ProjectMagic->InitMagic(this, ProjectileDamage);
	//		UE_LOG(LogTemp, Warning, TEXT("Projectile Init"));
	//	}
	//}
}

void ALionKnight::UseMagic()
{
	if (ProjectileMagic)
	{
		FVector MagicStartLoc{ GetActorLocation() + (GetActorForwardVector() * 150.f) };

		AProjectileMagic* SpawnMagic = GetWorld()->SpawnActor<AProjectileMagic>(ProjectileMagic, MagicStartLoc, GetActorRotation());
		SpawnMagic->InitMagic(this, ProjectileDamage);
	}
}