// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/Enemy.h"
#include "Boss.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTestDelegate);
/**
 * 
 */
UCLASS()
class UNREALRPG_API ABoss : public AEnemy
{
	GENERATED_BODY()

protected:
	UFUNCTION(BlueprintCallable)
	void CallPage2Dispatcher()
	{
		BossPage2Dispatcher.Broadcast();
	}
	
protected:
	UPROPERTY(BlueprintAssignable)
	FTestDelegate BossPage2Dispatcher;
};
