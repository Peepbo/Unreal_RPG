// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/Enemy.h"
#include "Engine/DataTable.h"
#include "Boss.generated.h"

USTRUCT(BlueprintType)
struct FBossStatDataTable : public FEnemyStatDataTable
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaximumMentality;
};

USTRUCT(BlueprintType)
struct FBossSkillSet : public FEnemySkillSet
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FEnemySpecialAttack> DodgeMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FEnemySpecialAttack> BackAttackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FEnemyMagicAttack> MagicAttackMontage;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FBossDelegate);
/**
 * 
 */
UCLASS()
class UNREALRPG_API ABoss : public AEnemy
{
	GENERATED_BODY()

protected:
	UFUNCTION(BlueprintCallable)
	void CallPhase2Dispatcher()
	{
		BossPhase2Dispatcher.Broadcast();
	}
	void CallDieDispatcher()
	{
		BossDieDispatcher.Broadcast();
	}

private:
	/* DataTable에 저장된 SkillSet을 가져오는 함수 */
	virtual void InitAttackMontage() override;
	
protected:
	UPROPERTY(BlueprintAssignable)
	FBossDelegate BossPhase2Dispatcher;
	UPROPERTY(BlueprintAssignable)
	FBossDelegate BossDieDispatcher;

private:

};
