// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MeleeAnimInstance.h"
#include "PlayerAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class UNREALRPG_API UPlayerAnimInstance : public UMeleeAnimInstance
{
	GENERATED_BODY()

public:


public:
	virtual void InitializeAnimationProperties() override;

	virtual void UpdateAnimationProperties(float DeltaTime) override;
	
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	class APlayerCharacter* PlayerCharacter;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	bool bLockOn;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	FVector PlayerForward;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	FVector2D MoveValue;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float MoveAngle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	bool bIsMove;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	bool bRoll;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	FVector2D LastRollMoveValue;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	bool bBackDodge;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true"))
	bool bDrinkingPotion;


	/* IK_Foot 전용 변수 */
	UPROPERTY(BlueprintReadOnly, Category = "IK_Foot", meta = (AllowPrivateAccess = "true"))
	FVector IKLeftFootEffector;

	UPROPERTY(BlueprintReadOnly, Category = "IK_Foot", meta = (AllowPrivateAccess = "true"))
	FVector IKRightFootEffector;

	UPROPERTY(BlueprintReadOnly, Category = "IK_Foot", meta = (AllowPrivateAccess = "true"))
	float IKHipOffset;

	UPROPERTY(BlueprintReadOnly, Category = "IK_Foot", meta = (AllowPrivateAccess = "true"))
	FRotator IKLeftFootRotator;

	UPROPERTY(BlueprintReadOnly, Category = "IK_Foot", meta = (AllowPrivateAccess = "true"))
	FRotator IKRightFootRotator;

	/* Fast Path 전용 변수 */

	/* Forward ( angle <= abs(90) || LockOn ), Backward ( angle > abs(90)) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Fast Path", meta = (AllowPrivateAccess = "true"))
	bool bSelectBrakeForward;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Fast Path", meta = (AllowPrivateAccess = "true"))
	bool bSelectJogForward;
};
