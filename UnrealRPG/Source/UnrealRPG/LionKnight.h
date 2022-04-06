// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy.h"
#include "LionKnight.generated.h"

/**
 * 
 */
UCLASS()
class UNREALRPG_API ALionKnight : public AEnemy
{
	GENERATED_BODY()
	
public:
	ALionKnight();

private:
	virtual	void PlayAttackMontage() override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
