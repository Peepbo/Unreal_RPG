// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Patrolable.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI, meta = (CannotImplementInterfaceInBlueprint))
class UPatrolable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class UNREALRPG_API IPatrolable
{
	GENERATED_BODY()
	// Add interface functions to this class. This is the class that will be inherited to implement this interface.

public:
	UFUNCTION(BlueprintCallable, Category = "Patrol")
	virtual TArray<FVector> GetPatrolPath() = 0;

	UFUNCTION(BlueprintCallable, Category = "Patrol")
	virtual void InitPatrolPath(int32 Size) = 0;

	UFUNCTION(BlueprintCallable, Category = "Patrol")
	virtual void DrawPatrolPath(float DrawTime = 0.f) = 0;

	/* 에디터에서 부를 수 있게 구현하여 Path를 Update한다. */
	UFUNCTION(BlueprintCallable, Category = "Patrol")
	virtual void RetargetPatrolPath() = 0;
};
