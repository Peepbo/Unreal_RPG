// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MeleeCharacter.generated.h"

UENUM(BlueprintType)
enum class ECombatState : uint8 
{
	ECS_Unoccupied UMETA(DisplayName = "Unoccupied"),
	ECS_Attack UMETA(DisplayName = "Attack"),

	ECS_MAX UMETA(DisplayName = "DefaultMax")
};

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

	/* 앞이나 뒤로 이동할 때 호출하는 함수 */
	void MoveForward(float Value);

	/* 오른쪽이나 왼쪽으로 이동할 때 호출하는 함수 */
	void MoveRight(float Value);

	/*
	* 주어진 속도로 카메라 좌우를 회전하는 함수
	* @param Rate	정규화된 값이며 1.f는 100%를 의미합니다.
	*/
	void TurnAtRate(float Rate);

	/*
	* 주어진 속도로 카메라 상하를 회전하는 함수
	* @param Rate	정규화된 값이며, 1.f는 100%를 의미합니다.
	*/
	void LookUpAtRate(float Rate);

	/* 공격 함수 */
	void Attack();

	/* 공격 버튼을 눌렀을 때 호출하는 함수 */
	void PressedAttack();

	/* 공격 버튼을 땠을 때 호출하는 함수 */
	void ReleasedAttack();

	/* 콤보를 이어나갈지 멈출지 확인하는 함수 */
	UFUNCTION(BlueprintCallable)
	void CheckComboAttack();

	/* 공격을 멈출 때 호출하는 함수 */
	UFUNCTION(BlueprintCallable)
	void EndAttack();

	/* 다음 콤보를 진행하는지 검사하는 함수 */
	void CheckComboTimer();

	/* 콤보 타이머를 실행하는 함수 */
	UFUNCTION(BlueprintCallable)
	void StartComboTimer();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	/* 카메라를 달아 놓을 카메라 팔 추가 (카메라와 캐릭터 사이에 일정 거리를 두기위해 만든 컴포넌트) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/* 캐릭터를 따라 움직이는 카메라 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	/* 좌우를 둘러보는 감도 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float BaseTurnRate;

	/* 상하를 둘러보는 감도 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float BaseLookUpRate;

	/* 캐릭터의 상태 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	ECombatState CombatState;

	/* 공격 몽타주 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class UAnimMontage* AttackMontage;

	/* 공격 버튼이 눌렸는지 안눌렸는지 */
	bool bPressedAttackButton;

	/* 콤보 타이머 */
	FTimerHandle ComboTimer;

	/* true: 연속 공격 진행, false: 연속 공격 종료 */
	bool bShouldComboAttack;

	/* 콤보 상황 */
	UPROPERTY(VisibleAnywhere, Category = Combat, meta = (AllowPrivateAccess = "true"))
	int32 AttackCombo;

	/* 최대 콤보 */
	UPROPERTY(EditDefaultsOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	int32 MaximumAttackCombo;

public:

	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE bool GetAttacking() const { return CombatState == ECombatState::ECS_Attack; }
};
