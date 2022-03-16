// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "Engine/DataTable.h"
#include "RPGTypes.h"
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AttackDamage;

	UPROPERTY(EditDefaultsOnly)
	float ShieldScale;
};

UCLASS()
class UNREALRPG_API AShield : public AItem
{
	GENERATED_BODY()
public:
	AShield();

protected:
	virtual void OnConstruction(const FTransform& Transform) override;

public:
	void InitPushShiledData(bool bDebugVisible);

private:
	bool ShieldTraceSingle(bool bDebugVisible, FHitResult& OutHit);

	UFUNCTION()
	void PushShield(bool bDebugVisible);
	
private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = DataTable, meta = (AllowPrivateAccess = "true"))
	UDataTable* DataTable;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = DataTable, meta = (AllowPrivateAccess = "true"))
	int32 ShieldItemNumber;

	/* 아이템 이름, 아이콘, 모델은 AItem에 정의되어있음 */
	float ShieldDefence;
	/* 방패가 막을 수 있는 각도 */
	float DefenceDegree;

	float AttackDamage;

	FTimerDelegate ShieldDelegate;

	FName ShieldSocketName;

public:
	FORCEINLINE float GetDefenceDegree() const { return DefenceDegree; }
	FORCEINLINE FTimerDelegate GetPushShieldDelegate() { return ShieldDelegate; }
};
