// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/EnemyAnimInstance.h"
#include "LionKnightAnimInstance.generated.h"

class ALionKnight;
/**
 * 
 */
UCLASS()
class UNREALRPG_API ULionKnightAnimInstance : public UEnemyAnimInstance
{
	GENERATED_BODY()
public:
	virtual void InitializeAnimationProperties() override;

	virtual void UpdateAnimationProperties(float DeltaTime) override;
	
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Class", meta = (AllowPrivateAccess = "true"))
	ALionKnight* LionKnight;
};
