// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy.h"
#include "DarkKnight.generated.h"

/**
 * 
 */
UCLASS()
class UNREALRPG_API ADarkKnight : public AEnemy
{
	GENERATED_BODY()
	
public:

protected:
	virtual void BeginPlay() override;

private:

	class UKnightAnimInstance* AnimInstance;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	TArray<UAnimMontage*> AttackMontage;
};
