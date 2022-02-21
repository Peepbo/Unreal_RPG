// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "Engine/DataTable.h"
#include "WeaponType.h"

#include "Weapon.generated.h"

/**
 * 
 */

USTRUCT(BlueprintType)
struct FWeaponDataTable : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString WeaponName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EWeaponType WeaponType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float WeaponDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UTexture2D* WeaponIcon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USkeletalMesh* WeaponMesh;
};

UCLASS()
class UNREALRPG_API AWeapon : public AItem
{
	GENERATED_BODY()
public:
	AWeapon();

	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = DataTable, meta = (AllowPrivateAccess = "true"))
	UDataTable* WeaponDataTable;

	// 아이템 이름, 아이콘, 모델은 AItem에 있음
	// 그외 type, damage 등을 정의함
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = DataTable, meta = (AllowPrivateAccess = "true"))
	EWeaponType WeaponType;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = DataTable, meta = (AllowPrivateAccess = "true"))
	float WeaponDamage;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* WeaponCollision;

public:
	FORCEINLINE UBoxComponent* GetWeaponCollision() const { return WeaponCollision; }
};
