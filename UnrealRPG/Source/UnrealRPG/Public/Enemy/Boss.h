// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/Enemy.h"
#include "Engine/DataTable.h"
#include "Boss.generated.h"

USTRUCT(BlueprintType)
struct FBossSkillSet : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FEnemyAdvancedAttack> AdvancedAttackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FEnemySpecialAttack> SpecialAttackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FEnemyAdvancedAttack> DodgeMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FEnemySpecialAttack> BackAttackMontage;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite)
	//	TSubclassOf<AProjectileMagic> ProjectileMagic;
	//
	//UPROPERTY(EditAnywhere, BlueprintReadWrite)
	//	float ProjectileMagicDamage;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FBossPageUpDelegate);
/**
 * 
 */
UCLASS()
class UNREALRPG_API ABoss : public AEnemy
{
	GENERATED_BODY()

protected:
	UFUNCTION(BlueprintCallable)
	void CallPage2Dispatcher()
	{
		BossPage2Dispatcher.Broadcast();
	}

private:
	/* DataTable에 저장된 SkillSet을 가져오는 함수 */
	virtual void InitAttackMontage() override;
	
protected:
	UPROPERTY(BlueprintAssignable)
	FBossPageUpDelegate BossPage2Dispatcher;

	/* 상대가 뒤에 있을 때 실행하는 공격 */
	TDoubleLinkedList<FEnemySpecialAttack> BackAttackList;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = DataTable, meta = (AllowPrivateAccess = "true"))
	UDataTable* SkillSetDataTable;

private:

};
