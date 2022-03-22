// Fill out your copyright notice in the Description page of Project Settings.


#include "KnightAnimInstance.h"
#include "DarkKnight.h"
#include "Kismet/KismetMathLibrary.h"

void UKnightAnimInstance::InitializeAnimationProperties()
{
	Super::InitializeAnimationProperties();

	DarkKnight = Cast<ADarkKnight>(TryGetPawnOwner());
}

void UKnightAnimInstance::UpdateAnimationProperties(float DeltaTime)
{
	Super::UpdateAnimationProperties(DeltaTime);

	if (DarkKnight == nullptr) {
		DarkKnight = Cast<ADarkKnight>(TryGetPawnOwner());
	}

	if (DarkKnight) {
		bIsBattleMode = DarkKnight->GetBattleMode();
		bIsSprint = DarkKnight->GetSprinting();

		WalkDirection = DarkKnight->GetWalkDirection();

		bWalkDirectionZero = UKismetMathLibrary::NearlyEqual_FloatFloat(WalkDirection, 0.f);

		bTurnInPlace = DarkKnight->GetTurnInPlace();

		bTurnLeft = DarkKnight->GetTurnLeft();
	}
}