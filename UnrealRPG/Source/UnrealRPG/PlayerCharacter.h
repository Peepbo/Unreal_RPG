// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MeleeCharacter.h"
#include "DamageState.h"

#include "PlayerCharacter.generated.h"

/**
 * 
 */
DECLARE_MULTICAST_DELEGATE(FEnemyDamageTypeResetDelegate);

UCLASS()
class UNREALRPG_API APlayerCharacter : public AMeleeCharacter
{
	GENERATED_BODY()
	
public:
	APlayerCharacter();

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

	/* 위의 TurnAtRate와 동일 (마우스 전용) */
	void TurnAtRateInMouse(float Rate);
	/* 위의 LookUpAtRate와 동일 (마우스 전용) */
	void LookUpAtRateInMouse(float Rate);

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

	UFUNCTION(BlueprintCallable)
		void SaveDegree();

	void BeginAttackRotate(float DeltaTime);

	void PressedBattleModeChange();

	/* 강 공격 버튼 관련 함수 */
	void PressedChargedAttack();
	void ReleasedChargedAttack();

	void PrepareChargedAttack();

	UFUNCTION(BlueprintCallable)
		void ChargedAttack();

	void ResetAttack();

	UFUNCTION(BlueprintCallable)
		void EndChargedAttack();

	/* 공격 검사를 시작하는 함수 */
	UFUNCTION(BlueprintCallable)
		void StartAttackCheckTime(bool bWeaponAttack = true);
	/* 공격 검사를 종료하는 함수 */
	UFUNCTION(BlueprintCallable)
		void EndAttackCheckTime();

	/* SphereTraceSingle을 사용하여 검사 후, Hit 정보를 바탕으로 데미지를 준다. */
	UFUNCTION()
	void TracingAttackSphere(bool bWeaponAttack, float Damage, FName StartSocket, FName EndSocket);

	/* 공격 시 스태미나를 사용할 때 호출되는 함수, 누를 때가 아닌 공격 검사가 시작될 때 호출됨 */
	UFUNCTION(BlueprintCallable)
		void UseStaminaToAttack(bool bWeaponAttack = true);

	/* 강 공격 버튼 관련 함수 */
	void PressedLockOn();

	/* 락온에 해당하는 적들 중 가장 가까운 적을 반환함, 해당하는 적이 없으면 nullptr 반환 */
	class AEnemy* GetNearestEnemyWithLockOn(const TArray<AActor*> Actors);

	/* 락온 위치로 카메라를 회전하는 함수, (RLerp 사용) */
	void RotateCameraByLockOn();

	/* bIsShieldImpact을 false로 바꾸는 함수 */
	UFUNCTION(BlueprintCallable)
	void EndShieldImpact();

	/* impact state를 끝내는 함수 */
	UFUNCTION(BlueprintCallable)
	void EndDamageInpact();

	/* 대쉬 공격 */
	void DashAttack();

	/* 발차기 공격 */
	void KickAttack();

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

private:
	/* ThumbStick의 Axis를 가져온다. */
	const FVector2D GetThumbStickAxis();
	/* ThumbStick의 Degree를 가져온다. */
	const float GetThumbStickDegree();

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

	/* 공격 버튼이 눌렸는지 안눌렸는지 */
	bool bPressedAttackButton;

	/* 콤보 타이머 (loop) */
	FTimerHandle ComboTimer;

	/* true: 연속 공격 진행, false: 연속 공격 종료 */
	UPROPERTY(VisibleAnywhere, Category=Combat,meta=(AllowPrivateAccess ="true"))
	bool bShouldComboAttack;

	/* 콤보 상황 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
		int32 AttackCombo;

	/* Steminar */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Stat, meta = (AllowPrivateAccess = "true"))
		float ST;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Stat, meta = (AllowPrivateAccess = "true"))
		float MaximumST;

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
		UAnimMontage* AttackMontage;

	/* 구르기 몽타주 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
		UAnimMontage* RollMontage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
		UAnimMontage* RollBackMontage;
	

	/* 매번 AnimInstance를 검사하지않고 캐싱하여 재사용 */ // fix umelee -> uplayer
	class UPlayerAnimInstance* AnimInstance;

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
	bool bPressedChargedAttackButton;

	/*
	* Enemy의 DamageTypeReset을 모아둘 멀티캐스트 델리게이트
	* 사용 이유 : 여러번 공격되는 것을 방지
	*/
	FEnemyDamageTypeResetDelegate EnemyDamageTypeResetDelegate;

	FTimerHandle VelocityChecker;

	/* 공격 전 회전을 할 지 */
	UPROPERTY(BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
		bool bIsBeforeAttackRotate;

	/* 컨트롤러가 가리키는 월드 방향 */
	FRotator SaveRotator;

	/* 회전(공격 전) 속도 */
	UPROPERTY(EditDefaultsOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
		float BeforeAttackRotateSpeed;

	/* 차지 준비, 공격 몽타주 */
	UPROPERTY(EditDefaultsOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
		UAnimMontage* ReadyToChargedAttackMontage;
	UPROPERTY(EditDefaultsOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
		UAnimMontage* ChargedAttackMontage;

	/* 강 공격인지 아닌지, 해당 변수의 상태에 따라 데미지 적용 값이 바뀐다. */
	bool bIsChargedAttack;
	/* 다음 공격이 강 공격인지 아닌지, 콤보가 유지됬는데 강 공격이면 강 공격 함수를 호출한다. */
	bool bShouldChargedAttack;

	bool bIsReadyToChargedAttack;

	/* 구르기에 필요한 스태미나 */
	float RollRequiredStamina;

	/* 공격 충돌 확인 타이머, 타이머가 작동되면 특정 딜레이(=0.005f) 마다 저장된 함수를 호출함 */
	FTimerHandle AttackCheckTimer;

	/* 락온 상태인지 아닌지 */
	UPROPERTY(VisibleAnywhere,Category=Combat,meta=(AllowPrivateAccess="true"))
	bool bLockOn;

	/* 락온 상태일 때 -1,0,+1 (왼쪽 ,그 외, 오른쪽) */
	int32 LockOnAxis;

	/* 락온 시 visible이 켜져야되는 위젯, 해당 위젯의 월드 위치를 카메라(컨트롤러)가 lookAt함 */
	class UWidgetComponent* LockOnWidgetData;
	/* 최소 Pitch 값, 몬스터 크기에 따라 달라지는 값(원활한 전투 시야를 위해 추가함) */
	float MinimumLockOnPitchValue;

	/* SphereOverlapActors에 필요한 Array (락온 전용), 미리 캐쉬해둠 */
	TArray<TEnumAsByte<EObjectTypeQuery>> TraceObjectTypes;

	FVector2D MoveValue;
	FVector LastHitPoint;

	bool bIsShieldImpact;

	/* 피해 몽타주 모음 */
	UPROPERTY(EditDefaultsOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	TArray<UAnimMontage*> ImpactMontages;

	/* 공격 몽타주의 Section 이름 */
	FName Combo1Name;
	FName Combo2Name;
	FName Combo3Name;

	/* AttackMontage를 강제로 종료할 때 BlendOutValue */
	UPROPERTY(EditDefaultsOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float StopAttackMontageBlendOutValue;

	/* DashAttack의 몽타주 */
	UPROPERTY(EditDefaultsOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* DashAttackMontage;

	/* AttackMontage의 최대 검사 횟수 */
	UPROPERTY(EditDefaultsOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	int32 MaximumAttackIndex;

	/* 발차기 몽타주 */
	UPROPERTY(EditDefaultsOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* KickAttackMontage;

	/* 발차기 데미지 */
	float KickDamage;

	/* 무기의 TopSocketName, 공격 검사 시 필요 */
	UPROPERTY(EditDefaultsOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	FName WeaponTopSocketName;
	/* 무기의 BottomSocketName, 공격 검사 시 필요 */
	UPROPERTY(EditDefaultsOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	FName WeaponBottomSocketName;
	
	/* 발차기의 Socket, 발차기 공격 검사 시 필요 */
	UPROPERTY(EditDefaultsOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	FName KickFootSocketName;

	/* 발차기 소모 스태미나 */
	float KickStamina;

public:
	FORCEINLINE bool GetLockOn() const { return bLockOn; }
	FORCEINLINE FVector2D GetMoveValue() const { return MoveValue; }
	FORCEINLINE FVector2D GetThumStickAxisForce() const { return { GetInputAxisValue("MoveForward"), GetInputAxisValue("MoveRight") }; }
	/* 방어 시 필요한 정보를 Enemy한테 전달받는다 */
	FORCEINLINE void SetHitPoint(const FVector HitPoint) { LastHitPoint = HitPoint; }
	FORCEINLINE ECombatState GetCombatState() const { return CombatState; }
	FORCEINLINE bool GetShiledImpact() const { return bIsShieldImpact; }
};
