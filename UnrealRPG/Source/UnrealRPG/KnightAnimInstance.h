// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyAnimInstance.h"
#include "KnightAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class UNREALRPG_API UKnightAnimInstance : public UEnemyAnimInstance
{
	GENERATED_BODY()
public:
	virtual void InitializeAnimationProperties() override;

	virtual void UpdateAnimationProperties(float DeltaTime) override;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Class", meta = (AllowPrivateAccess = "true"))
	class ADarkKnight* DarkKnight;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	bool bShouldDrawWeapon;
};
