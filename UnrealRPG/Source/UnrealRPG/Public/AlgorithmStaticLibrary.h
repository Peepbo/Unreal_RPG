// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "AlgorithmStaticLibrary.generated.h"

/**
 * 
 */
UCLASS()
class UNREALRPG_API UAlgorithmStaticLibrary : public UObject
{
	GENERATED_BODY()

public:

	/* Fisher-Yates shuffle Algorithm */
	template<class T>
	static FORCEINLINE void ShuffleArray(TArray<T>& Array)
	{
		int32 Random, Order = Array.Num() - 1;

		for (int Index = Order; Index > 0; Index--)
		{
			Random = FMath::RandRange(0, Order);
			Swap(Array[Index], Array[Random]);
		}
	}
};
