// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "DamageState.h"

#include "MeleeCharacter.generated.h"

UENUM(BlueprintType)
enum class ECombatState : uint8 
{
	ECS_Unoccupied UMETA(DisplayName = "Unoccupied"),
	ECS_Attack UMETA(DisplayName = "Attack"),
	ECS_Roll UMETA(DisplayName = "Roll"),
	ECS_Guard UMETA(DisplayName = "Guard"),

	ECS_MAX UMETA(DisplayName = "DefaultMax")
};

DECLARE_MULTICAST_DELEGATE(FEnemyDamageTypeResetDelegate); 

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
	void Attack(int32 MontageIndex = 0);
	void SubAttack();

	/* 공격 버튼 관련 함수 */
	void PressedAttack();
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

	/* 기본 무기 생성 */
	class AWeapon* SpawnDefaultWeapon();

	class AShield* SpawnDefaultShield();

	/* 무기 장착 */
	void EquipWeapon(AWeapon* Weapon, bool bSwapping = false);

	void EquipShield(AShield* Shield, bool bSwapping = false);

	/* 일단 캐릭터가 보는 방향으로 구르기 */
	void Roll();

	UFUNCTION(BlueprintCallable)
	void EndRoll();

	/* 구르기 버튼 관련 함수 */
	void PressedRoll();
	void ReleasedRoll();

	void Sprint();
	void EndSprint(bool bChangeState = false);

	/* 착용 아이템 아이콘 변경 함수 (블루프린트에서 작성) */
	UFUNCTION(BlueprintImplementableEvent)
	void UpdateLeftItemIcon();
	UFUNCTION(BlueprintImplementableEvent)
	void UpdateRightItemIcon();

	/* 스태미나 회복 */
	void RecoverStamina();

	/* 스태미나 회복 타이머 */
	void StartStaminaRecoveryTimer();
	void StopStaminaRecoveryTimer();

	/* 스태미나 회복 지연 타이머 */
	void StartStaminaRecoveryDelayTimer();

	/* 스태미나 감소 */
	void ReduceStamina();

	/* 스태미나 감소 타이머 */
	void StartStaminaReductionTimer();
	void StopStaminaReductionTimer();

	/* 달리기 버튼 관련 함수 */
	void PressedSprint();
	void ReleasedSprint();

	/* 오른손 무기 오버랩 함수 */
	UFUNCTION()
	void OnRightWeaponOverlap(
		class UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	/* 
	* 데미지 타입 리셋 함수
	* EnemyDamageTypeResetDelegate에 들어가있는 함수를 모두 호출시킴 
	*/
	UFUNCTION(BlueprintCallable)
	void ResetDamageState();

	/* 보조 공격(가드) 버튼 관련 함수 */
	void PressedSubAttack();
	void ReleasedSubAttack();

	void EndSubAttack();

	void CheckVelocity();

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

	/* 공격 버튼이 눌렸는지 안눌렸는지 */
	bool bPressedAttackButton;

	/* 콤보 타이머 (loop) */
	FTimerHandle ComboTimer;

	/* true: 연속 공격 진행, false: 연속 공격 종료 */
	bool bShouldComboAttack;

	/* 콤보 상황 */
	UPROPERTY(VisibleAnywhere, Category = Combat, meta = (AllowPrivateAccess = "true"))
	int32 AttackCombo;

	/* Health Point */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Stat, meta = (AllowPrivateAccess = "true"))
	float HP;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Stat, meta = (AllowPrivateAccess = "true"))
	float MaximumHP;
	/* Attack Damage */
	UPROPERTY(VisibleAnywhere, Category = Stat, meta = (AllowPrivateAccess = "true"))
	float AD;
	
	/* Steminar */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Stat, meta = (AllowPrivateAccess = "true"))
	float ST;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Stat, meta = (AllowPrivateAccess = "true"))
	float MaximumST;
	///* Ability Power */
	//UPROPERTY(VisibleAnywhere, Category = Stat, meta = (AllowPrivateAccess = "true"))
	//float AP;
	///* Defense */
	//UPROPERTY(VisibleAnywhere, Category = Stat, meta = (AllowPrivateAccess = "true"))
	//float DEF;

	/* 타입의 안정성을 보장해주는 템플릿 클래스, 기본 무기를 설정하는 곳 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AWeapon> DefaultWeaponClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AShield> DefaultShieldClass;

	/* 플레이어가 착용하고있는 무기 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	AWeapon* EquippedWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	AShield* EquippedShield;

	/* 공격 몽타주 모음 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	TArray<UAnimMontage*> AttackMontages;

	/* 구르기 몽타주 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* RollMontage;

	/* 캐릭터가 스프린트 상태인지 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	bool bIsSprint;

	/* 최대 기본 속도 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float MaximumWalkSpeed;
	/* 최대 스프린트 속도 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float MaximumSprintSpeed;

	/* 매번 AnimInstance를 검사하지않고 캐싱하여 재사용 */
	class UMeleeAnimInstance* AnimInstance;

	/* 스태미나 회복 타이머, 스태미나를 채울 수 있을 때 작동하는 타이머 (loop) */
	FTimerHandle StaminaRecoveryTimer;

	/* 스태미나 회복 지연 타이머, 지연 시간 이후 스태미나 타이머를 작동시키는 딜레이 타이머 */
	FTimerHandle StaminaRecoveryDelayTimer;
	
	/* 스태미나 회복 지연 시간, 일반 상태에서 회복 상태로 바뀌는 시간 */
	float StaminaRecoveryDelayTime;

	/* 스태미나를 감소시키는 타이머 (loop) */
	FTimerHandle StaminaReductionTimer;

	bool bPressedSprintButton;

	bool bPressedRollButton;

	bool bPressedSubAttackButton;
	/* 
	* Enemy의 DamageTypeReset을 모아둘 멀티캐스트 델리게이트
	* 사용 이유 : 여러번 공격되는 것을 방지
	*/
	FEnemyDamageTypeResetDelegate EnemyDamageTypeResetDelegate;

	FTimerHandle VelocityChecker;

public:

	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE bool GetAttacking() const { return CombatState == ECombatState::ECS_Attack; }
	FORCEINLINE bool GetSprinting() const { return bIsSprint; }
	FORCEINLINE float GetMaximumSpeed() const { return MaximumSprintSpeed; }
	FORCEINLINE bool GetGuarding() const { return CombatState == ECombatState::ECS_Guard; }
};
