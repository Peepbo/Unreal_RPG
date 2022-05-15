// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Magic/Magic.h"
#include "RPGTypes.h"
#include "EnemyAdvancedAttackManager.generated.h"

USTRUCT(BlueprintType)
struct FEnemyNormalAttack
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UAnimMontage* AttackMontage;

	/* 공격 가능 최대 거리 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float AttackAbleDistance;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	EAttackType AttackType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float AttackDamage;
};

USTRUCT(BlueprintType)
struct FEnemySpecialAttack : public FEnemyNormalAttack
{
	GENERATED_USTRUCT_BODY()

	/* 다시 반복하기 까지 필요한 대기 횟수, 일반 공격의 경우 0으로 지정 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	int32 MaximumWaitCount;

	int32 WaitCount;

	/* 최소 시작할 때 WaitCount를 MaximumWaitCount로 초기화 할 지 안할지 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	bool bResetWaitCount = true;
};

USTRUCT(BlueprintType)
struct FEnemyMagicAttack : public FEnemyNormalAttack
{
	GENERATED_USTRUCT_BODY()

	/* 다시 반복하기 까지 필요한 대기 횟수, 일반 공격의 경우 0으로 지정 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	int32 MaximumWaitCount;

	int32 WaitCount;

	/* 최소 시작할 때 WaitCount를 MaximumWaitCount로 초기화 할 지 안할지 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	bool bResetWaitCount = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<AMagic> Magic;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float MagicDamage;
};
/**
 * 
 */
UCLASS()
class UNREALRPG_API UEnemyAdvancedAttackManager : public UObject
{
	GENERATED_BODY()
	
public:
	UEnemyAdvancedAttackManager();

	void InitMontageData(TArray<FEnemyNormalAttack> NormalAttack, TArray<FEnemySpecialAttack> SpecialAttack, TArray<FEnemySpecialAttack> BackAttack, TArray<FEnemyMagicAttack> MagicAttack);
	/* NormalAttack을 제외하고 실행 우선 순위를 정한다. (Normal은 로직 마지막에 자동 추가됨, 같은 타입 혹은 Normal, Max type을 추가 시 NormalType만 순위에 지정)*/
	void SetAttackSequence(const TArray<EEnemyMontageType>& Sequence);

	void ChooseAttack(float ToTargetDegree, float ToTargetDistance);
	bool CheckAttackPossible(const EEnemyMontageType& Type, float ToTargetDegree, float ToTargetDistance);

private:
	TDoubleLinkedList<FEnemyNormalAttack> NormalAttackList;
	TDoubleLinkedList<FEnemySpecialAttack> SpecialAttackList;
	TDoubleLinkedList<FEnemySpecialAttack> BackAttackList;
	TDoubleLinkedList<FEnemyMagicAttack> MagicAttackList;

	bool bSequenceShuffle = false;

	TArray<EEnemyMontageType> TypeSequence;


	FEnemyNormalAttack* AttackCommonDataPtr = nullptr;
	/* 추가 정보들 */
	EEnemyMontageType MontageType = EEnemyMontageType::EEMT_Normal;
	TSubclassOf<AMagic> LastMagic;
	float LastMagicDamage;
	
public:
	FORCEINLINE void SetShuffle(bool bNext) { bSequenceShuffle = bNext; }

	FORCEINLINE bool IsValidAttack() const { return AttackCommonDataPtr != nullptr; }
	FORCEINLINE EEnemyMontageType GetMontageType() const { return MontageType; }
	FORCEINLINE EAttackType GetAttackType() const { return AttackCommonDataPtr != nullptr ? AttackCommonDataPtr->AttackType : EAttackType::EAT_MAX; }
	FORCEINLINE UAnimMontage* GetMontage() const { return AttackCommonDataPtr != nullptr ? AttackCommonDataPtr->AttackMontage : nullptr; }
	FORCEINLINE float GetAttackDamage() const { return AttackCommonDataPtr != nullptr ? AttackCommonDataPtr->AttackDamage : 0.f; }
	FORCEINLINE float GetAttackableDistance() const { return AttackCommonDataPtr != nullptr ? AttackCommonDataPtr->AttackAbleDistance : 0.f; }
	FORCEINLINE TSubclassOf<AMagic> GetLastMagic() const { return LastMagic != nullptr ? LastMagic : nullptr; }
	FORCEINLINE float GetLastMagicDamage() const { return LastMagicDamage; }
};
