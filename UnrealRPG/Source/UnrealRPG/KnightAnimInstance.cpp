// Fill out your copyright notice in the Description page of Project Settings.


#include "KnightAnimInstance.h"
#include "Enemy.h"

void UKnightAnimInstance::NativeInitializeAnimation()
{
	Enemy = Cast<AEnemy>(TryGetPawnOwner());
}