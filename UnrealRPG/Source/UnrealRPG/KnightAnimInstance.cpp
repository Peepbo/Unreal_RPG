// Fill out your copyright notice in the Description page of Project Settings.


#include "KnightAnimInstance.h"
#include "DarkKnight.h"

void UKnightAnimInstance::InitializeAnimationProperties()
{
	Super::InitializeAnimationProperties();

	DarkKnight = Cast<ADarkKnight>(TryGetPawnOwner());
}

void UKnightAnimInstance::UpdateAnimationProperties(float DeltaTime)
{
	Super::UpdateAnimationProperties(DeltaTime);
}