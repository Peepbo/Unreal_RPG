// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MeleeCharacter.h"
#include "RPGTypes.h"
#include "Enemy.generated.h"

USTRUCT(BlueprintType)
struct FEnemyAdvancedAttack
{
	GENERATED_BODY()

		UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		class UAnimMontage* AttackMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		float AttackAbleRange;
};

UCLASS()
class UNREALRPG_API AEnemy : public AMeleeCharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/* HealthBar 위젯을 숨긴다. */
	UFUNCTION(BlueprintImplementableEvent)
	void HideHealthBar();

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
		void ChangeBattleMode();

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

	void TracingAttackSphere();

	UFUNCTION(BlueprintCallable)
		void StartAttackCheckTime();

	UFUNCTION(BlueprintCallable)
		void EndAttackCheckTime();

	UFUNCTION(BlueprintCallable)
		void FaceOff(float NextWalkDirection);

	UFUNCTION(BlueprintCallable)
		void EndFaceOff();

	void StartRestTimer();

	void EndRestTimer();

	UFUNCTION(BlueprintCallable)
		void ChangeSprinting(bool IsSprinting);

	virtual void FindCharacter();

	UFUNCTION(BlueprintCallable)
		float GetDegreeForwardToTarget();


	UFUNCTION(BlueprintCallable)
		void StartAttack();

	UFUNCTION(BlueprintCallable)
		void EndAttack();

	UFUNCTION(BlueprintCallable)
		void GetWeaponMesh(class USkeletalMeshComponent* ItemMesh);

	UFUNCTION(BlueprintCallable)
			void PlayMontage(class UAnimMontage* Montage);

	UFUNCTION(BlueprintCallable)
	const float GetAttackableDistance() const;

protected:
	class UAnimInstance* AnimInstance;

	class AEnemyAIController* EnemyAIController;

	/* 전투 이동속도 관련 변수 (RootMotion을 사용하지 않는 Enemy가 사용) */
	float BattleWalkSpeed;
	float BattleRunSpeed;

	FTimerHandle RestTimer;

	bool bRestTime;

	bool bAvoidImpactState;

	/* 플레이어를 저장하는 변수 */
	UPROPERTY(BlueprintReadOnly, Category = "Behavior Tree", meta = (AllowPrivateAccess = "true"))
	class APlayerCharacter* Target;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
		TArray<FEnemyAdvancedAttack> AdvancedAttackMontage;

	APlayerCharacter* OverlapCharacter;

	FTimerHandle SearchTimer;

	int32 AttackIndex;
	int32 LastAttackIndex;

private:

	/* 데미지를 받을 수 있는 상태인지를 검사하기 위해 사용하는 변수 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	EDamageState DamageState;

	/* AI관련 행동을 저장하는 트리 */
	UPROPERTY(EditAnywhere, Category = "Behavior Tree", meta = (AllowPrivateAccess = "true"))
	class UBehaviorTree* BehaviorTree;

	/* 정찰 지점1 */
	UPROPERTY(EditAnywhere, Category = "Behavior Tree", meta = (AllowPrivateAccess = "true", MakeEditWidget = "true"))
	FVector PatrolPoint;

	/* 정찰 지점2 */
	UPROPERTY(EditAnywhere, Category = "Behavior Tree", meta = (AllowPrivateAccess = "true", MakeEditWidget = "true"))
	FVector PatrolPoint2;

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

	/* 몬스터 크기를 대략적으로 나타내는 enum 변수 */
	EEnemySize EnemySize;

	/* 락온 시 최소 Pitch값을 나타내는 변수, 크기에 따라 달라짐 */
	float LockOnMinimumPitchValue;

	UPROPERTY(EditDefaultsOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* DeathMontage;

	bool bLockOnEnemy;

	UPROPERTY(EditDefaultsOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	bool bPatrolEnemy;


	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		FRotator LastSaveRotate;

	bool bTurn;

	bool bAttackable;

	UPROPERTY(VisibleAnywhere, Category = Combat, meta = (AllowPrivateAccess = "true"))
		USkeletalMeshComponent* WeaponMesh;

	FTimerHandle AttackCheckTimer;

	/* -1 : left, 0 : forward, 1 : right */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
		float WalkDirection;

	/* true: 왼쪽, false: 오른쪽 */
	bool bTurnLeft;

	bool bMove;

	UPROPERTY(EditDefaultsOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
		float InplaceRotateSpeed;

	UPROPERTY(EditDefaultsOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
		float AttackRotateSpeed;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION()
	void ResetDamageState() { (bDying ? DamageState = EDamageState::EDS_invincibility : DamageState = EDamageState::EDS_Unoccupied); }
	UFUNCTION()
	void ResetLockOn() { bLockOnEnemy = false; }

	FORCEINLINE UBehaviorTree* GetBehaviorTree() const { return BehaviorTree; }
	FORCEINLINE bool GetBattleMode() const { return bIsBattleMode; }
	FORCEINLINE bool GetSprinting() const { return bIsSprint; }
	FORCEINLINE void SetSprinting(bool IsSprint) { bIsSprint = IsSprint; }
	FORCEINLINE UWidgetComponent* GetLockOnWidget() const { return LockOnWidget; }
	FORCEINLINE float GetMinimumLockOnPitchValue() const { return LockOnMinimumPitchValue; }
	
	FORCEINLINE bool DamageableState() const { return DamageState == EDamageState::EDS_Unoccupied; }
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
};
