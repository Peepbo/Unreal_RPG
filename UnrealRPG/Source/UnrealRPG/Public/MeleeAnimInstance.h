// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "RPGTypes.h"
#include "MeleeAnimInstance.generated.h"

class AMeleeCharacter;

UCLASS()
class UNREALRPG_API UMeleeAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	virtual void InitializeAnimationProperties();

	/* 매 프레임 업데이트가 필요한 함수, Blueprint의 EventGraph에서 사용 */
	UFUNCTION(BlueprintCallable)
	virtual void UpdateAnimationProperties(float DeltaTime);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float Speed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	FVector2D Velocity;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float ZVelocity;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	FVector2D AttackExclusionVelocity;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float AttackExclusionSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float UnoccupiedSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float LastRelativeVelocityAngle;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	FVector2D ForwardVector;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	bool bIsInAir;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	bool bIsAccelerating;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true"))
	bool bIsSprint;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true"))
	bool bIsGuard;

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

	/* impact중 한번 더 impact 됬을 때 사용하는 변수 */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Animation, meta = (AllowPrivateAccess = "true"))
	bool bShouldPlayShieldImpact;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true"))
	bool bIsDamageImpact;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true"))
	FVector LastHitDirection;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true"))
	EAttackType LastDamagedAttackType;

private:
	UPROPERTY()
	AMeleeCharacter* Character;

public:
	FORCEINLINE float GetSpeed() const { return Speed; }
	FORCEINLINE bool GetAccelerating() const { return bIsAccelerating; }
	FORCEINLINE void SetLastRelativeVelocityAngle(float Angle) { LastRelativeVelocityAngle = Angle; }
	FORCEINLINE void SetShouldPlayShieldImpact(bool bCondition) { bShouldPlayShieldImpact = bCondition; }
};
