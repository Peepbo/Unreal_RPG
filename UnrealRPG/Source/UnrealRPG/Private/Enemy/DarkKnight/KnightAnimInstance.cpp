// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/DarkKnight/KnightAnimInstance.h"
#include "Enemy/DarkKnight/DarkKnight.h"
#include "Kismet/KismetMathLibrary.h"

void UKnightAnimInstance::InitializeAnimationProperties()
{
	Super::InitializeAnimationProperties();

	if (DarkKnight == nullptr) 
	{
		DarkKnight = Cast<ADarkKnight>(TryGetPawnOwner());
	}
}

void UKnightAnimInstance::UpdateAnimationProperties(float DeltaTime)
{
	Super::UpdateAnimationProperties(DeltaTime);

	if (DarkKnight)
	{
		bShouldDrawWeapon = DarkKnight->ShouldDrawWeapon();

		if (DarkKnight->GetMove())
		{
			MoveValue = 0.5f;
			if (DarkKnight->GetSprinting()) 
			{
				MoveValue = 1.f;
			}
		}
		else 
		{
			MoveValue = 0.f;
		}
	}
}