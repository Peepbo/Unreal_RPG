// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "MeleeAnimInstance.generated.h"

/** 클래스 기능
 * 애니메이션 관련 변수들을 선언하고 변경되는 값에 따라
 * 애니메이션이 바뀌게 함
 */
UCLASS()
class UNREALRPG_API UMeleeAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
//public:
//	virtual void NativeInitializeAnimation() override;

public:
	UFUNCTION(BlueprintCallable)
		virtual void InitializeAnimationProperties();

	/* 매 프레임 업데이트가 필요한 함수, Blueprint의 EventGraph에서 사용 */
	UFUNCTION(BlueprintCallable)
		virtual void UpdateAnimationProperties(float DeltaTime);

protected:

	/* 캐릭터의 속도 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float Speed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	FVector2D Velocity;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	FVector2D AttackExclusionVelocity;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float AttackExclusionSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float LastRelativeVelocityAngle;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	FVector2D ForwardVector;

	/* 캐릭터가 공중에 있는지 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	bool bIsInAir;

	/* 캐릭터가 움직이고 있는지 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	bool bIsAccelerating;

	/* 캐릭터가 Sprint 상태인지 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true"))
	bool bIsSprint;

	/* 캐릭터가 Guard 상태인지 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true"))
	bool bIsGuard;

	/* 캐릭터가 전투 모드 상태인지 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true"))
	bool bIsBattleMode;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Animation, meta = (AllowPrivateAccess = "true"))
	bool bReadyToBattle;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Animation, meta = (AllowPrivateAccess = "true"))
	bool bAttack;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Animation, meta = (AllowPrivateAccess = "true"))
	bool bDying;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true"))
		bool bIsShieldImpact;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true"))
		bool bIsDamageImpact;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true"))
		FVector LastHitDirection;

private:
	class AMeleeCharacter* Character;

public:
	FORCEINLINE float GetSpeed() const { return Speed; }
	FORCEINLINE bool GetAccelerating() const { return bIsAccelerating; }
};
