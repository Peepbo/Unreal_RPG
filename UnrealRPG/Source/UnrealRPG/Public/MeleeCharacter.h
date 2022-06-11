// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "RPGTypes.h"
#include "MeleeCharacter.generated.h"

class UAnimMontage;
class UCharacterMovementComponent;
class USoundCue;
class USoundAttenuation;

UCLASS()
class UNREALRPG_API AMeleeCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMeleeCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/* 캐릭터가 데미지를 입었을 때 호출되는 함수 */


	/* Shield Function */
	UFUNCTION(BlueprintCallable)
	virtual void EndShieldImpact();


	/* Damage Function */
	UFUNCTION(BlueprintImplementableEvent)
	void RespondTakeDamage(float DamageAmount, AActor* DamageCauser, EAttackType AttackType);

	UFUNCTION(BlueprintCallable)
	virtual void EndDamageImpact();

	virtual bool CustomTakeDamage(float DamageAmount, AActor* DamageCauser, EAttackType AttackType);

	UFUNCTION(BlueprintCallable)
	virtual bool FallingDamage(float LastMaxmimumZVelocity);

	UFUNCTION(BlueprintCallable)
	void ChangeDamageState(EDamageState State) { DamageState = State; }



	UFUNCTION(BlueprintCallable)
	void ChangeCombatState(ECombatState NextCombatState);

	void ChangeMoveState(bool bNextMoveSprinting);

	/* 자연스럽게 속도를 조절하기위해 사용하는 함수 (목표 속도로 점차 바뀜) */
	void ChangeMaximumSpeedForSmoothSpeed(float DeltaTime);

	/* 강제로 Sprint를 종료하는 함수, Sprint 키는 누른 상태지만 캐릭터 이동키를 안눌렀을 때 호출 */
	virtual void HardResetSprint();

	UFUNCTION(BlueprintCallable)
	virtual	FVector GetFootLocation(bool bLeft);

	/* 캐릭터 회전값에 상대적인 가속 방향을 AnimInstance로 전송하는 함수 */
	UFUNCTION(BlueprintCallable)
	void SendRelativeVelocityAngle();

	UFUNCTION(BlueprintCallable)
	float GetHpPercentage();

	UFUNCTION(BlueprintCallable)
	void ForceDie();

protected:
	/* 캐릭터의 상태 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	ECombatState CombatState;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* DieMontage;

	/* Health Point */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Stat, meta = (AllowPrivateAccess = "true"))
	float HP;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Stat, meta = (AllowPrivateAccess = "true"))
	float MaximumHP;
	/* Attack Damage */
	UPROPERTY(VisibleAnywhere, Category = Stat, meta = (AllowPrivateAccess = "true"))
	float AD;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float MaximumWalkSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float MaximumSprintSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	bool bIsBattleMode;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	bool bDying;

	UPROPERTY(VisibleAnywhere, Category = Combat, meta = (AllowPrivateAccess = "true"))
	bool bSprinting;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	UParticleSystem* BloodParticle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	USoundCue* BloodSound;

	UPROPERTY(EditDefaultsOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	USoundCue* LastBloodSound;

	/* 공격 시 TraceSphere를 시각화할지 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	bool bVisibleTraceSphere;

	bool bIsShieldImpact;
	
	FVector LastHitDirection;

	EAttackType LastDamagedAttackType;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stat DataTable", meta = (AllowPrivateAccess = "true"))
	float RewardGold;

	/* 데미지를 받을 수 있는 상태인지를 검사하기 위해 사용하는 변수 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	EDamageState DamageState;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Custom Damage function
	UFUNCTION(BlueprintCallable)
	bool CustomApplyDamage(AActor* DamagedActor, float DamageAmount, AActor* DamageCauser, EAttackType AttackType);

	/* 공격한 대상이 사망했을 때 호출되는 함수 */
	virtual void TargetDeath(float TargetRewardGold);

private:

	bool ChangeSpeed;

public:
	UFUNCTION()
	void ResetDamageState() { (bDying ? DamageState = EDamageState::EDS_invincibility : DamageState = EDamageState::EDS_Unoccupied); }
	FORCEINLINE bool DamageableState() const { return DamageState == EDamageState::EDS_Unoccupied; }

	FORCEINLINE bool GetSprinting() const { return bSprinting; }
	FORCEINLINE ECombatState GetCombatState() const { return CombatState; }
	FORCEINLINE bool GetAttacking() const { return CombatState == ECombatState::ECS_Attack; }
	FORCEINLINE bool GetGuarding() const { return CombatState == ECombatState::ECS_Guard; }
	FORCEINLINE bool GetImpacting() const { return CombatState == ECombatState::ECS_Impact; }
	FORCEINLINE bool GetShiledImpact() const { return bIsShieldImpact; }
	FORCEINLINE void SetShiledImpact(bool NextBool) { bIsShieldImpact = NextBool; }
	FORCEINLINE bool GetBattleMode() const { return bIsBattleMode; }
	FORCEINLINE bool GetDying() const { return bDying; }
	FORCEINLINE UParticleSystem* GetBloodParticle() const { return BloodParticle; }
	FORCEINLINE USoundCue* GetBloodSound() const { return BloodSound; }
	FORCEINLINE USoundCue* GetLastBloodSound() const { return LastBloodSound; }
	FORCEINLINE FVector GetLastHitDirection() const { return LastHitDirection; }
	FORCEINLINE EAttackType GetLastDamagedAttackType() const { return LastDamagedAttackType; }
};
