// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MeleeCharacter.h"
#include "RPGTypes.h"

#include "PlayerCharacter.generated.h"

/**
 * 
 */
class UKismetMathLibrary;

UCLASS()
class UNREALRPG_API APlayerCharacter : public AMeleeCharacter
{
	GENERATED_BODY()
	
public:
	APlayerCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/* Move Function */
	void MoveForward(float Value);
	void MoveRight(float Value);

	/* Camera Function */
	void TurnAtRate(float Rate);
	void LookUpAtRate(float Rate);

	/* TurnAtRate와 동일 (마우스 전용) */
	void TurnAtRateInMouse(float Rate);
	/* LookUpAtRate와 동일 (마우스 전용) */
	void LookUpAtRateInMouse(float Rate);

	
	/* Button Function */
	void PressedAttack();
	void ReleasedAttack();

	void PressedSubAttack();
	void ReleasedSubAttack();

	void PressedRollAndSprint();
	void ReleasedRollAndSprint();

	void PressedChargedAttack();
	void ReleasedChargedAttack();

	void EndSprint();

	/* Attack Function */
	void MainAttack();
	void SubAttack();
	
	/* 콤보를 이어나갈지 멈출지 확인하는 함수 */
	UFUNCTION(BlueprintCallable)
		bool CheckComboAttack();

	/* 공격을 멈출 때 호출하는 함수 */
	UFUNCTION(BlueprintCallable)
		void EndAttack();

	/* 다음 콤보를 진행하는지 검사하는 함수 */
	void CheckComboTimer();

	/* 콤보 타이머를 실행하는 함수 */
	UFUNCTION(BlueprintCallable)
		void StartComboTimer();

	/* 데미지 타입 리셋 함수, EnemyDamageTypeResetDelegate에 들어가있는 함수를 모두 호출시킴 */
	UFUNCTION(BlueprintCallable)
		void ResetDamageState();

	void EndSubAttack();

	UFUNCTION(BlueprintCallable)
		void SaveDegree();

	void BeginAttackRotate(float DeltaTime);

	void PrepareChargedAttack();

	UFUNCTION(BlueprintCallable)
		void ChargedAttack();

	void ResetAttack();

	/* 공격 검사를 시작하는 함수 */
	UFUNCTION(BlueprintCallable)
		void StartAttackCheckTime();
	/* 공격 검사를 종료하는 함수 */
	UFUNCTION(BlueprintCallable)
		void EndAttackCheckTime();

	/* 대쉬 공격 */
	void DashAttack();

	/* Item Function */
	/* 기본 장비 생성 */
	class AWeapon* SpawnDefaultWeapon();
	class AShield* SpawnDefaultShield();
	class APotion* SpawnDefaultPotion();

	/* 장비 장착 */
	void EquipWeapon(AWeapon* Weapon, bool bSwapping = false);
	void EquipShield(AShield* Shield, bool bSwapping = false);
	void EquipPotion(APotion* Potion, bool bSwapping = false);

	/* 착용 아이템 아이콘 변경 함수 (블루프린트에서 작성) */
	UFUNCTION(BlueprintImplementableEvent)
		void UpdateLeftItemIcon();
	UFUNCTION(BlueprintImplementableEvent)
		void UpdateRightItemIcon();

	UFUNCTION(BlueprintCallable)
		void PrepareShieldAttack();


	/* Stamina Function */
	/* 스태미나 회복 */
	void RecoverStamina();

	/* 스태미나 회복 타이머 */
	void StartStaminaRecoveryTimer();
	void StopStaminaRecoveryTimer();

	/* 스태미나 회복 지연 타이머 */
	void StartStaminaRecoveryDelayTimer();

	/* Roll Function */
	/* 일단 캐릭터가 보는 방향으로 구르기 */
	void Roll();

	UFUNCTION(BlueprintCallable)
		void EndRoll();


	/* Lock-On Function */
	void PressedLockOn();

	/* 락온에 해당하는 적들 중 가장 가까운 적을 반환함, 해당하는 적이 없으면 nullptr 반환 */
	class AEnemy* GetNearestEnemyWithLockOn(const TArray<AActor*> Actors);

	/* 락온 위치로 카메라를 회전하는 함수, (RLerp 사용) */
	void RotateCameraByLockOn();


	/* Usable Item Function */
	void PressedUseItem();
	void ReleasedUseItem();

	void UseItem();

	void SkipUseItem();

	UFUNCTION(BlueprintCallable)
	void DrinkPotion();

	UFUNCTION(BlueprintCallable)
	void EndUseItem();

	UFUNCTION(BlueprintCallable)
	bool CheckLand();

	UFUNCTION(BlueprintCallable)
	void JumpLandAttack();

	UFUNCTION(BlueprintCallable)
		void SaveMaxmimumVelocity();

	UFUNCTION(BlueprintCallable)
		void ResetZVelocity() { MaximumZVelocity = 0.f; }

	UFUNCTION(BlueprintCallable)
		void SetHitPoint_Test(FVector HitPoint) { LastHitPoint = HitPoint; }

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	/* 델리게이트에 Enemy의 데미지 타입 초기화 함수를 넣는다 */
	void AddFunctionToDamageTypeResetDelegate(AEnemy* Enemy, const FName& FunctionName);

	/* Lock-On을 종료하는 함수 */
	void ResetLockOn();

	float GetMoveAngle();

private:
	virtual void HardResetSprint() override;

	virtual void EndShieldImpact() override;

	/* ThumbStick의 Axis를 가져온다. (local direction) */
	const FVector2D GetThumbStickLocalAxis();
	/* ThumbStick의 Degree를 가져온다. */
	const float GetThumbStickDegree();
	/* ThumbStick방향과 Controller방향을 혼합한 Axis를 가져온다. (world direction) */
	const FVector2D GetThumbStickWorldAxis();

	void Sprint();

	void StopDelayForRoll();

	/* True : 몽타주 실행 중, False : 몽타주 실행하지 않음 */
	bool CheckMontageState();
	void StopAllMontage();
	/* Montage가 플레이중일 때 강제로 멈추게함, 아닐경우 아무 효과 없음 */
	void ForceStopAllMontage();

	/* Jump */
	void PressedJump();
	void ReleasedJump();

	void PrepareJumpAttack();

	/* IK_Foot Function */
	void UpdateIKFootData(float DeltaTime);
	void IKFootTrace(const FName& SocketName, FHitResult& HitResult);
	void ContinueUpdateIKData(float DeltaTime);

private:
	/* Camera Variable */
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

	UPROPERTY(EditDefaultsOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		float LockOnCameraSpeed;


	/* Button Variable */
	/* 공격 버튼이 눌렸는지 안눌렸는지 */
	bool bPressedAttackButton;
	bool bPressedRollAndSprintButton;
	bool bPressedSubAttackButton;
	bool bPressedChargedAttackButton;
	bool bPressedUseItemButton;
	bool bPressedJumpButton;


	/* Timer Variable */
	/* 콤보 타이머 (loop) */
	FTimerHandle ComboTimer;

	/* 스태미나 회복 타이머, 스태미나를 채울 수 있을 때 작동하는 타이머 (loop) */
	FTimerHandle StaminaRecoveryTimer;

	/* 스태미나 회복 지연 타이머, 지연 시간 이후 스태미나 타이머를 작동시키는 딜레이 타이머 */
	FTimerHandle StaminaRecoveryDelayTimer;

	/* 스태미나를 감소시키는 타이머 (loop) */
	FTimerHandle StaminaReductionTimer;

	/* 공격 충돌 확인 타이머, 타이머가 작동되면 특정 딜레이(=0.005f) 마다 저장된 함수를 호출함 */
	FTimerHandle AttackCheckTimer;

	FTimerHandle SprintAndRollPlayTimer;

	FTimerHandle RollDelayTimer;


	/* Stat Variable */
	/* Steminar */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Stat, meta = (AllowPrivateAccess = "true"))
		float ST;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Stat, meta = (AllowPrivateAccess = "true"))
		float MaximumST;

	/* 플레이어가 착용하고있는 무기 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Item, meta = (AllowPrivateAccess = "true"))
		AWeapon* EquippedWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Item, meta = (AllowPrivateAccess = "true"))
		AShield* EquippedShield;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Item, meta = (AllowPrivateAccess = "true"))
		APotion* EquippedPotion;


	/* Montage Variable */
	/* 공격 몽타주 모음 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
		TArray<UAnimMontage*> MainComboMontages;

	/* 차지 몽타주 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
		TArray<UAnimMontage*> ChargedComboMontages;

	/* DashAttack 몽타주 */
	UPROPERTY(EditDefaultsOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
		UAnimMontage* DashAttackMontage;

	/* JumpAttack 몽타주 */
	UPROPERTY(EditDefaultsOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* PrepareJumpAttackMontage;

	UPROPERTY(EditDefaultsOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* JumpAttackMontage;


	/* Default Item Variable */
	/* 타입의 안정성을 보장해주는 템플릿 클래스, 기본 무기를 설정하는 곳 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Item, meta = (AllowPrivateAccess = "true"))
		TSubclassOf<AWeapon> DefaultWeaponClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Item, meta = (AllowPrivateAccess = "true"))
		TSubclassOf<AShield> DefaultShieldClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Item, meta = (AllowPrivateAccess = "true"))
		TSubclassOf<APotion> DefaultPotionClass;


	/* Attack Variable */
	/* true: 연속 공격 진행, false: 연속 공격 종료 */
	UPROPERTY(VisibleAnywhere, Category=Combat,meta=(AllowPrivateAccess ="true"))
	bool bShouldContinueAttack;

	/* 콤보 공격 인덱스 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
		int32 ComboAttackMontageIndex;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
		int32 ChargedComboAttackMontageIndex;

	/* Enemy의 DamageTypeReset을 모아둘 멀티캐스트 델리게이트, 여러번 공격되는 것을 방지 */
	FEnemyDamageTypeResetDelegate EnemyDamageTypeResetDelegate;

	/* Enemy의 LockOn Reset함수를 담을 델리게이트, 대상이 죽었을 때 락온된 대상인지 검사하여 락온을 리셋 */
	FEnemyLockOnResetDelegate EnemyLockOnResetDelegate;

	/* 공격 전 회전을 할 지 */
	UPROPERTY(BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
		bool bIsBeforeAttackRotate;

	/* 컨트롤러가 가리키는 월드 방향 */
	FRotator SaveRotator;

	/* 회전(공격 전) 속도 */
	UPROPERTY(EditDefaultsOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
		float BeforeAttackRotateSpeed;

	/* 강 공격인지 아닌지, 해당 변수의 상태에 따라 데미지 적용 값이 바뀐다. */
	bool bIsChargedAttack;
	/* 다음 공격이 강 공격인지 아닌지, 콤보가 유지됬는데 강 공격이면 강 공격 함수를 호출한다. */
	bool bShouldChargedAttack;

	bool bIsReadyToChargedAttack;

	/* AttackMontage를 강제로 종료할 때 BlendOutValue */
	UPROPERTY(EditDefaultsOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
		float StopAttackMontageBlendOutValue;

	/* AttackMontage의 최대 검사 횟수 */
	UPROPERTY(EditDefaultsOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
		int32 MaximumAttackIndex;

	EPlayerAttackType PlayerAttackType;

	EWeaponAttackType WeaponAttackType;

	UPROPERTY(EditDefaultsOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float StopAttackMontageBlendOut;


	/* Stamina Variable */
	/* 스태미나 회복 지연 시간, 일반 상태에서 회복 상태로 바뀌는 시간 */
	float StaminaRecoveryDelayTime;

	/* 구르기에 필요한 스태미나 */
	float RollRequiredStamina;


	/* Lock-On Variable */
	/* 락온 상태인지 아닌지 */
	UPROPERTY(VisibleAnywhere, Category = Combat, meta = (AllowPrivateAccess = "true"))
		bool bLockOn;

	/* 락온 시 visible이 켜져야되는 위젯, 해당 위젯의 월드 위치를 카메라(컨트롤러)가 lookAt함 */
	class UWidgetComponent* LockOnWidgetData;
	/* 최소 Pitch 값, 몬스터 크기에 따라 달라지는 값(원활한 전투 시야를 위해 추가함) */
	float MinimumLockOnPitchValue;

	/* SphereOverlapActors에 필요한 Array (락온 전용), 미리 캐쉬해둠 */
	TArray<TEnumAsByte<EObjectTypeQuery>> TraceObjectTypes;


	/* Damage Variable */
	FVector LastHitPoint;

	/* Animation Instance Variable */
	/* 매번 AnimInstance를 검사하지않고 캐싱하여 재사용 */ // fix umelee -> uplayer
	class UPlayerAnimInstance* AnimInstance;

	FVector2D MoveValue;
	FVector2D LastMoveValue;
	FVector2D LastRollMoveValue;

	bool bBackDodge;

	bool bCanRoll;
	
	UPROPERTY(EditDefaultsOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float RollDelay;

	/* 점프 착지 시 필요함 */
	float MaximumZVelocity;

	/* Usable Item Variable */
	bool bDrinkingPotion;

	
	/* IK_Foot Variable */
	float IKLeftFootOffset;
	float IKRightFootOffset;
	float IKHipOffset;
	float IKTraceDistance;
	float IKInterpSpeed;
	FRotator IKLeftFootRotator;
	FRotator IKRightFootRotator;
	FName LeftFootSocketName;
	FName RightFootSocketName;
	float IKFootAlpha;

public:
	FORCEINLINE bool GetLockOn() const { return bLockOn; }
	FORCEINLINE FVector2D GetThumStickAxisForce() const { return { GetInputAxisValue("MoveForward"), GetInputAxisValue("MoveRight") }; }
	/* 방어 시 필요한 정보를 Enemy한테 전달받는다 */
	FORCEINLINE void SetHitPoint(const FVector HitPoint) { LastHitPoint = HitPoint; }
	FORCEINLINE ECombatState GetCombatState() const { return CombatState; }

	FORCEINLINE FVector2D GetLastRollMoveValue() const { return LastRollMoveValue; }
	FORCEINLINE bool GetBackDodge() const { return bBackDodge; }
	FORCEINLINE bool GetDrinking() const { return bDrinkingPotion; }

	FORCEINLINE float GetMaximumZValue() const { return MaximumZVelocity; }

	/* IK_Foot 전용 인라인 함수 */
	FORCEINLINE float GetIKLeftFootOffset() const { return IKLeftFootOffset; }
	FORCEINLINE float GetIKRightFootOffset() const { return IKRightFootOffset; }
	FORCEINLINE float GetIKHipOffset() const { return IKHipOffset; }
	FORCEINLINE FRotator GetIKLeftFootRotator() const { return IKLeftFootRotator; }
	FORCEINLINE FRotator GetIKRightFootRotator() const { return IKRightFootRotator; }
	FORCEINLINE float GetIKFootAlpha() const { return IKFootAlpha; }
};
