// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "KnightAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class UNREALRPG_API UKnightAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;

private:
	class AEnemy* Enemy;

	float Speed;
	bool bIsInAir;

public:
};
