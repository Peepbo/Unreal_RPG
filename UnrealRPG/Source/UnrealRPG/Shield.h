// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "Engine/DataTable.h"
#include "Shield.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FShieldDataTable : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString ShieldName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ShieldDefence;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UTexture2D* ShieldIcon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USkeletalMesh* ShieldMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DefenceDegree;

	UPROPERTY(EditDefaultsOnly)
	float ShieldScale;
};

UCLASS()
class UNREALRPG_API AShield : public AItem
{
	GENERATED_BODY()

protected:
	virtual void OnConstruction(const FTransform& Transform) override;
	
private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = DataTable, meta = (AllowPrivateAccess = "true"))
	UDataTable* DataTable;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = DataTable, meta = (AllowPrivateAccess = "true"))
	int32 ShieldTier;

	/* 아이템 이름, 아이콘, 모델은 AItem에 정의되어있음 */
	float ShieldDefence;
	/* 방패가 막을 수 있는 각도 */
	float DefenceDegree;
public:
	FORCEINLINE float GetDefenceDegree() const { return DefenceDegree; }
};
