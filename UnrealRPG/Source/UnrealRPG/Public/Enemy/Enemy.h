// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MeleeCharacter.h"
#include "RPGTypes.h"
#include "EnemyAdvancedAttackManager.h"
#include "Engine/DataTable.h"
#include "Enemy.generated.h"

class APlayerCharacter;
class AExecutionArea;
class AMagic;
class UAnimMontage;

USTRUCT(BlueprintType)
struct FEnemyStatDataTable : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName CharacterName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaximumHP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float RewardGold;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName LockOnSocketName = "Hips";

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EEnemySize EnemySize;
};

USTRUCT(BlueprintType)
struct FEnemySkillSet : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FEnemyNormalAttack> NormalAttackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FEnemySpecialAttack> SpecialAttackMontage;
};

UCLASS()
class UNREALRPG_API AEnemy : public AMeleeCharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemy();

public:
	void PlayTakeExecutionMontage();
	void ClearStunTimer();
	FVector GetStunAttackLocation();
	UFUNCTION(BlueprintCallable)
	void ChangePawnCollision(bool bBlock);
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/* 탐색 시아 (범위 안에 들어오면 전투가 활성화된다.) */
	UFUNCTION()
	virtual void AgroSphereOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	UFUNCTION()
		virtual void AgroSphereEndOverlap(
			UPrimitiveComponent* OverlappedComponent,
			AActor* OtherActor,
			UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex);

	/* 공격 시작 범위 */
	UFUNCTION()
		void CombatRangeOverlap(
			UPrimitiveComponent* OverlappedComponent,
			AActor* OtherActor,
			UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex,
			bool bFromSweep,
			const FHitResult& SweepResult);

	/* 공격 범위 벗어남 */
	UFUNCTION()
		void CombatRangeEndOverlap(
			UPrimitiveComponent* OverlappedComponent,
			AActor* OtherActor,
			UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex);

	/* non-battle에서 battle로, 혹은 battle에서 non-battle로 */
	UFUNCTION(BlueprintCallable)
		virtual void ChangeBattleMode();

	UFUNCTION(BlueprintCallable)
		virtual	void PlayAttackMontage();

	/* 캐릭터의 크기를 변경하는 함수 (LockOn에 필요) */
	void ChangeEnemySize(EEnemySize Size);

	/* 비 전투/전투 콜라이더 세팅 전환 함수 */
	void ChangeColliderSetting(bool bBattle);

	UFUNCTION(BlueprintCallable)
		void Die();


	/* 이동된 함수 */
	UFUNCTION(BlueprintCallable)
		void SaveTargetRotator();

	UFUNCTION(BlueprintCallable)
		void StartRotate();

	UFUNCTION(BlueprintCallable)
		void StopRotate();

	UFUNCTION()
	void TracingAttackSphere(float Damage);

	UFUNCTION(BlueprintCallable)
		void StartAttackCheckTime(float DamagePersantage = 1.f);

	UFUNCTION(BlueprintCallable)
		void EndAttackCheckTime();

	UFUNCTION(BlueprintCallable)
		void FaceOff(float NextWalkDirection);

	UFUNCTION(BlueprintCallable)
		void EndFaceOff();

	UFUNCTION(BlueprintCallable)
	void Chase();

	void StartRestTimer();

	void EndRestTimer();

	UFUNCTION(BlueprintCallable)
		void ChangeSprinting(bool IsSprinting);

	virtual void FindCharacter();

	UFUNCTION(BlueprintCallable)
		float GetDegreeForwardToTarget();

	float GetDistanceToTarget();


	UFUNCTION(BlueprintCallable)
	void StartAttack();

	UFUNCTION(BlueprintCallable)
	virtual void EndAttack(bool bChooseNextAttack = true);

	UFUNCTION(BlueprintCallable)
	void GetWeaponMesh(class USkeletalMeshComponent* ItemMesh);

	UFUNCTION(BlueprintCallable)
	void PlayMontage(UAnimMontage* Montage);

	UFUNCTION(BlueprintCallable)
	const float GetAttackableDistance() const;

	void SetVisibleHealthBar(bool bVisible);
	void StartResetBattleModeTimer(float Delay);
	void StartResetTransformTimer(float Delay);

	void RecoverMentality();
	void StartRecoverMentalityDelayTimer(float Delay);

	UFUNCTION(BlueprintCallable)
	virtual void RecoverStun();
	void StartStunRecoveryTimer(float Delay);

	UFUNCTION(BlueprintCallable)
	void DestroyExecutionArea();

	virtual void ChooseNextAttack();

	virtual void InitAttackMontage();

	virtual void InitStat();

private:
	void CombatTurn(float DeltaTime);
	virtual void CheckCombatReset(float DeltaTime);
	UFUNCTION(BlueprintCallable)
	virtual void ResetCombat();
	void ResetTransform();

	virtual void TargetDeath(float RewardGold) override;
	UFUNCTION(BlueprintCallable)
	virtual void ActiveEnemy(APlayerCharacter* Player);

	UFUNCTION(BlueprintCallable)
	void CreateExecutionArea();


	UFUNCTION(BlueprintCallable)
	void EndBackAttack();

protected:
	class UAnimInstance* AnimInstance;

	class AEnemyAIController* EnemyAIController;

	FTimerHandle ResetBattleModeTimer;
	FTimerHandle EndCombatTimer;

	///* 일반 공격 관련 변수 (DataTable을 사용하지 않는 Enemy의 경우 사용) */
	//UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Combat)
	//TArray<FEnemyNormalAttack> NormalAttackMontage;
	//
	///* 특수 공격 관련 변수 (DataTable을 사용하지 않는 Enemy의 경우 사용) */
	//UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Combat)
	//TArray<FEnemySpecialAttack> SpecialAttackMontage;

	UPROPERTY()
	UEnemyAdvancedAttackManager* AttackManager;


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Setting)
	bool bUseSpecialAttack;
	
	/* 전투 이동속도 관련 변수 (RootMotion을 사용하지 않는 Enemy가 사용) */
	float BattleWalkSpeed;
	float BattleRunSpeed;

	FTimerHandle RestTimer;

	bool bRestTime;

	bool bAvoidImpactState;

	/* 플레이어를 저장하는 변수 */
	UPROPERTY(BlueprintReadOnly, Category = "Behavior Tree")
	APlayerCharacter* Target;


	APlayerCharacter* OverlapCharacter;

	FTimerHandle SearchTimer;

	

	UPROPERTY(EditDefaultsOnly, Category = Combat)
		float InplaceRotateSpeed;

	bool bTurn;

	/* 전투 지속 시간 (데미지를 입지 않거나, 데미지를 입히지 않으면 계속 올라감)*/
	float CombatResetTime;
	float MaximumCombatResetTime;
	float MaximumCombatDistance;

	FTransform FirstTransform;
	bool bPatrolableEnemy;

	/* 공격 몽타주를 처음에 섞을지 안섞을지 */
	UPROPERTY(EditAnywhere, Category = Combat)
	bool bRandomAttackMontage;

	/* 비전투 시간이 길어지면 자동으로 전투를 종료시킬지 (예를들어 플레이어가 계속 도망다니면 몬스터가 원래 위치로 이동하는 것, 보스 룸이 존재하는 보스는 false로 해줘야 함) */
	bool bAutoCombatReset;

	bool bDodge;

	UPROPERTY(EditDefaultsOnly, Category = Combat)
	float RestDelay;

	UPROPERTY(BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
		bool bStun;


	UPROPERTY(EditDefaultsOnly, Category = Setting, meta = (AllowPrivateAccess = "true"))
		FName EnemyDataTableRowName;

	/* Stat DataTable Variable */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stat DataTable", meta = (AllowPrivateAccess = "true"))
		UDataTable* EnemyStatDataTable;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stat DataTable", meta = (AllowPrivateAccess = "true"))
		FName CharacterName;

	UPROPERTY(VisibleAnywhere, Category = "Stat DataTable", meta = (AllowPrivateAccess = "true"))
		FName LockOnSocketName = "Hips";

	/* 몬스터 크기를 대략적으로 나타내는 enum 변수 */
	UPROPERTY(VisibleAnywhere, Category = "Stat DataTable", meta = (AllowPrivateAccess = "true"))
		EEnemySize EnemySize;

	/* 정신력, 데미지를 입으면 정신력이 깎이게 되며 2.5초동안 데미지를 입지 않으면 다시 정신력이 회복된다. */
	/* 정신력이 0 혹은 그 이하에 도달하면 기절 애니메이션이 실행되며 애니메이션(보스의 1->2페이지 전환 제외)에 상관없이 모든 애니메이션을 스킵하고 실행한다. */
	UPROPERTY(VisibleAnywhere, Category = Combat, meta = (AllowPrivateAccess = "true"))
		float Mentality;
	UPROPERTY(EditDefaultsOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
		float MaximumMentality;
	FTimerHandle MentalityRecoveryTimer;
	bool bRecoverMentality;

	/* Skill DataTable Variable */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill DataTable", meta = (AllowPrivateAccess = "true"))
		UDataTable* EnemySkillDataTable;

private:

	/* AI관련 행동을 저장하는 트리 */
	UPROPERTY(EditAnywhere, Category = "Behavior Tree", meta = (AllowPrivateAccess = "true"))
	class UBehaviorTree* BehaviorTree;

	/* 적(플레이어) 인식 콜라이더 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Combat, meta = (AllowPrivateAccess = "true"))
	class USphereComponent* AgroSphere;

	/* 공격 콜라이더 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Combat, meta = (AllowPrivateAccess = "true"))
	USphereComponent* CombatRangeSphere;

	/* 공격 범위안에 들어왔는지 */
	bool bInAttackRange;
	/* 달리기 상태인지 */
	bool bIsSprint;

	/* 락온되는 위치 및 이미지를 포함하는 위젯 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Combat, meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* LockOnWidget;


	/* 락온 시 최소 Pitch값을 나타내는 변수, 크기에 따라 달라짐 */
	float LockOnMinimumPitchValue;

	bool bLockOnEnemy;


	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		FRotator LastSaveRotate;

	bool bAttackable;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
		USkeletalMeshComponent* WeaponMesh;

	FTimerHandle AttackCheckTimer;

	/* -1 : left, 0 : forward, 1 : right */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
		float WalkDirection;

	/* true: 왼쪽, false: 오른쪽 */
	bool bTurnLeft;

	bool bMove;



	UPROPERTY(EditDefaultsOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
		float AttackRotateSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
		class UWidgetComponent* HealthBar;







	UPROPERTY(BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	bool bShouldStopStun;
	bool bPlaySpecialTakeDamage;

	FTimerHandle StunTimer;
	
	UPROPERTY(EditDefaultsOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* StunMontage;

	UPROPERTY(EditDefaultsOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* TakeExecutionMontage;

	UPROPERTY(EditDefaultsOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AExecutionArea> TakeExecutionAreaTemplate;
	UPROPERTY(BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	AExecutionArea* TakeExecutionArea;




public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual bool CustomTakeDamage(float DamageAmount, AActor* DamageCauser, EAttackType AttackType) override;

	void TakeMentalDamage(float DamageAmount);

	UFUNCTION()
	void ResetLockOn() { bLockOnEnemy = false; }

	FORCEINLINE UBehaviorTree* GetBehaviorTree() const { return BehaviorTree; }
	FORCEINLINE bool GetBattleMode() const { return bIsBattleMode; }
	FORCEINLINE bool GetSprinting() const { return bIsSprint; }
	FORCEINLINE void SetSprinting(bool IsSprint) { bIsSprint = IsSprint; }
	FORCEINLINE UWidgetComponent* GetLockOnWidget() const { return LockOnWidget; }
	FORCEINLINE float GetMinimumLockOnPitchValue() const { return LockOnMinimumPitchValue; }
	
	FORCEINLINE bool GetLockOn() const { return bLockOnEnemy; }
	FORCEINLINE void SetLockOn(bool NextBool) { bLockOnEnemy = NextBool; }

	FORCEINLINE float GetWalkDirection() const { return WalkDirection; }
	FORCEINLINE bool GetTurn() const { return bTurn; }

	UFUNCTION(BlueprintCallable)
		void SetMove(bool bNextBool) { bMove = bNextBool; }
	FORCEINLINE bool GetMove() const { return bMove; }

	UFUNCTION(BlueprintCallable)
	void SetTurnLeft(bool bNextTurn) { bTurnLeft = bNextTurn; }

	FORCEINLINE bool GetTurnLeft() const { return bTurnLeft; }

	FORCEINLINE USkeletalMeshComponent* GetWeapon() { return WeaponMesh; }

	FORCEINLINE bool ValidTakeExecutionMontage() const { return TakeExecutionMontage != nullptr; }

};
