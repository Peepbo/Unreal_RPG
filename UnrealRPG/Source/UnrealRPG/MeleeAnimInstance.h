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
public:
	virtual void NativeInitializeAnimation() override;

public:
	/* 매 프레임 업데이트가 필요한 함수, Blueprint의 EventGraph에서 사용 */
	UFUNCTION(BlueprintCallable)
	void UpdateAnimationProperties(float DeltaTime);

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	class AMeleeCharacter* MeleeCharacter;

	/* 캐릭터의 속도 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float Speed;

	/* 캐릭터가 공중에 있는지 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	bool bIsInAir;

	/* 캐릭터가 움직이고 있는지 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	bool bIsAccelerating;

	/* 캐릭터(머리)가 바라봐야 하는 Pitch */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Look, meta = (AllowPrivateAccess = "true"))
	float PitchOfLook;

	/* 캐릭터(머리)가 바라봐야 하는 Yaw */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Look, meta = (AllowPrivateAccess = "true"))
	float YawOfLook;

	/* 움직이지 않은체 공격하는지 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true"))
	bool bIsAttackWithoutMoving;

	/* 캐릭터가 Sprint 상태인지 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true"))
	bool bIsSprint;

	/* 캐릭터가 Guard 상태인지 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true"))
	bool bIsGuard;

	/* 캐릭터가 전투 모드 상태인지 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true"))
	bool bIsBattleMode;

public:
	FORCEINLINE float GetSpeed() const { return Speed; }
	FORCEINLINE bool GetAccelerating() const { return bIsAccelerating; }
};
