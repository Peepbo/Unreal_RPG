// Fill out your copyright notice in the Description page of Project Settings.


#include "LionKnight.h"

ALionKnight::ALionKnight()
{

}

void ALionKnight::PlayAttackMontage()
{
	if (AnimInstance)
	{
		//if (GetSprinting() && SprintAttackMontage)
		//{
		//	AnimInstance->Montage_Play(SprintAttackMontage);
		//	SetSprinting(false);
		//
		//	UE_LOG(LogTemp, Warning, TEXT("Play Sprint Attack Montage"));
		//}
		//else
		//{
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
		//}
	}
}

void ALionKnight::BeginPlay()
{
	Super::BeginPlay();

	ChangeEnemySize(EEnemySize::EES_Large);
}
