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

protected:
	//UFUNCTION(BlueprintCallable)
	//void FaceOff(int32 NextWalkDirection);
	//UFUNCTION(BlueprintCallable)
	//void EndFaceOff(int32 NextWalkDirection);
};
