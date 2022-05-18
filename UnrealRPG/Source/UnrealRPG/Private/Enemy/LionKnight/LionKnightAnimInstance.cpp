// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/LionKnight/LionKnightAnimInstance.h"
#include "Enemy/LionKnight/LionKnight.h"

void ULionKnightAnimInstance::InitializeAnimationProperties()
{
	Super::InitializeAnimationProperties();

	if (LionKnight == nullptr)
	{
		LionKnight = Cast<ALionKnight>(TryGetPawnOwner());
	}
}

void ULionKnightAnimInstance::UpdateAnimationProperties(float DeltaTime)
{
	Super::UpdateAnimationProperties(DeltaTime);
}
