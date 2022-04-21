// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "Engine/DataTable.h"
#include "RPGTypes.h"
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
	float NormalDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ChargedDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DashDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UTexture2D* WeaponIcon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USkeletalMesh* WeaponMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float NormalAttackStamina;
		
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DashAttackStamina;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ChargedAttackStamina;
};

UCLASS()
class UNREALRPG_API AWeapon : public AItem
{
	GENERATED_BODY()
public:
	AWeapon();

protected:
	virtual void OnConstruction(const FTransform& Transform) override;

public:
	void InitAttackData(EWeaponAttackType Type, bool bDebugVisible);

	/* 입력한 Type정보를 바탕으로 공격에 필요한 스태미나를 확인한다. */
	float GetRequiredStamina(EWeaponAttackType Type);

private:
	float GetDamage(EWeaponAttackType AttackType);

	bool WeaponTraceSingle(bool bDebugVisible, FHitResult& OutHit);

	UFUNCTION()
	void SwingWeapon(bool bDebugVisible);

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = DataTable, meta = (AllowPrivateAccess = "true"))
	UDataTable* WeaponDataTable;

	// 아이템 이름, 아이콘, 모델은 AItem에 있음
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = DataTable, meta = (AllowPrivateAccess = "true"))
	EWeaponType WeaponType;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = DataTable, meta = (AllowPrivateAccess = "true"))
	float NormalDamage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = DataTable, meta = (AllowPrivateAccess = "true"))
	float ChargedDamage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = DataTable, meta = (AllowPrivateAccess = "true"))
	float DashDamage;

	FName WeaponTopSocketName;
	FName WeaponBottomSocketName;

	FTimerDelegate AttackDelegate;

	EWeaponAttackType AttackType;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = DataTable, meta = (AllowPrivateAccess = "true"))
	float NormalAttackStamina;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = DataTable, meta = (AllowPrivateAccess = "true"))
	float DashAttackStamina;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = DataTable, meta = (AllowPrivateAccess = "true"))
	float ChargedAttackStamina;

public:
	FORCEINLINE FTimerDelegate GetAttackDelegate() { return AttackDelegate; }
};
