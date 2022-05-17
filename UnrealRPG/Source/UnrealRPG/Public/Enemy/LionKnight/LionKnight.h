// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/Boss.h"
#include "LionKnight.generated.h"

class UStaticMeshComponent;
class APlayerCharacter;
class AProjectileMagic;
class AMagic;

/**
 * 
 */
UCLASS()
class UNREALRPG_API ALionKnight : public ABoss
{
	GENERATED_BODY()
	
public:
	ALionKnight();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual void ResetCombat() override;

	virtual void EndAttack() override;

	virtual bool CustomTakeDamage(float DamageAmount, AActor* DamageCauser, EAttackType AttackType) override;

	virtual void InitStat() override;

	UFUNCTION(BlueprintCallable)
	AMagic* UseMagic();

	UFUNCTION(BlueprintCallable)
	void ReadyToBattle();

	UFUNCTION(BlueprintCallable)
	void GrapWeapon();

	void ResetBoss();

	UFUNCTION(BlueprintCallable)
	void EndDodge();

	UFUNCTION(BlueprintCallable)
	void EndEvent();

	/* Dodge가 가능한지 확인한다. TRUE? Play Dodge, FALSE? Not*/
	bool CheckDodge();

	/* 2페이지로 진입한다. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void NextPage();
	virtual void NextPage_Implementation();

	UFUNCTION(BlueprintNativeEvent)
	void InitPage();
	virtual void InitPage_Implementation();

	void PlayNextPageMontage();

	UFUNCTION(BlueprintCallable)
	void BreakGround();

	UFUNCTION(BlueprintCallable)
	void SplashDamage(bool bDefaultDamage, float SelectDamage);

	/* DataTable에 저장된 SkillSet을 가져오는 함수 */
	virtual void InitAttackMontage() override;

	virtual void RecoverStun() override;

private:
	bool bPrepareBattle;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Setting", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* GroundedWeapon;

	FTimerHandle ChangeWeaponSettingTimer;

	/* 3방향 회피  0? 왼쪽, 1? 오른쪽, 2? 후면 */
	TArray<FEnemySpecialAttack> DodgeMontage;
	TDoubleLinkedList<FEnemySpecialAttack> DodgeMontageList;

	/* 2페이지로 진입하는 몽타주 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Boss Page", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* NextPageMontage;

	/* 2페이지로 진입할 때 필요한 체력 퍼센트 */
	UPROPERTY(EditDefaultsOnly, Category = "Boss Page", meta = (AllowPrivateAccess = "true"))
	float NextPageStartHealthPercentage;

	/* 2페이지 인지 아닌지 */
	UPROPERTY(VisibleAnywhere, Category = "Boss Page", meta = (AllowPrivateAccess = "true"))
	bool bSecondPageUp;

	bool bEvent;

public:
	FORCEINLINE bool ShouldPrepareBattle() const { return bPrepareBattle; }
};
