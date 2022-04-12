// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "Engine/DataTable.h"
#include "RPGTypes.h"
#include "Potion.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FPotionDataTable : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString PotionName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UTexture2D* PotionIcon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class USkeletalMesh* PotionMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float RecoveryAmount;
};

UCLASS()
class UNREALRPG_API APotion : public AItem
{
	GENERATED_BODY()
public:
	APotion();

protected:
	virtual void OnConstruction(const FTransform& Transform) override;

public:
private:

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = DataTable, meta = (AllowPrivateAccess = "true"))
		UDataTable* DataTable;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = DataTable, meta = (AllowPrivateAccess = "true"))
		int32 PotionTier;


	float RecoveryAmount;
public:
	FORCEINLINE float GetRecoveryAmount() const { return RecoveryAmount; }
};