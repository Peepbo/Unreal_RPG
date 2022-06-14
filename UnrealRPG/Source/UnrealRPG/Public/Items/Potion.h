// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "Engine/DataTable.h"
#include "RPGTypes.h"
#include "Potion.generated.h"

class UTexture2D;
class USkeletalMesh;

USTRUCT(BlueprintType)
struct FPotionDataTable : public FTableRowBase
{
	GENERATED_BODY()

	
	FString PotionName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* PotionIcon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USkeletalMesh* PotionMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float RecoveryAmount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int MaximumCount;
};

UCLASS()
class UNREALRPG_API APotion : public AItem
{
	GENERATED_BODY()
public:
	APotion();

	void InitUseableCount();
	bool CheckUseableCount();
	void UsePotion();
protected:
	virtual void OnConstruction(const FTransform& Transform) override;

private:

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = DataTable, meta = (AllowPrivateAccess = "true"))
	UDataTable* DataTable;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = DataTable, meta = (AllowPrivateAccess = "true"))
	int32 PotionTier;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = DataTable, meta = (AllowPrivateAccess = "true"))
	float RecoveryAmount;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = DataTable, meta = (AllowPrivateAccess = "true"))
	int MaximumCount;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Potion, meta = (AllowPrivateAccess = "true"))
	int UseableCount;
public:
	FORCEINLINE float GetRecoveryAmount() const { return RecoveryAmount; }
};
