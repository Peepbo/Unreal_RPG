// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "RPGTypes.h"

#include "MeleeCharacter.generated.h"

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


	/* Shield Function */
	/* bIsShieldImpact을 false로 바꾸는 함수 */
	UFUNCTION(BlueprintCallable)
		virtual void EndShieldImpact();


	/* Damage Function */
	/* impact state를 끝내는 함수 */
	UFUNCTION(BlueprintCallable)
		virtual void EndDamageImpact();

	void ChangeMoveState(bool bNextMoveSprinting);

	virtual void HardResetSprint();

	void ChangeMaximumSpeedForSmoothSpeed(float DeltaTime);


	UFUNCTION(BlueprintCallable)
		bool CheckFootCollision(bool bLeft, FVector& Out_HitPoint);

	UFUNCTION(BlueprintCallable)
		void SaveRelativeVelocityAngle();

protected:
	/* 캐릭터의 상태 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
		ECombatState CombatState;

	/* Health Point */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Stat, meta = (AllowPrivateAccess = "true"))
		float HP;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Stat, meta = (AllowPrivateAccess = "true"))
		float MaximumHP;
	/* Attack Damage */
	UPROPERTY(VisibleAnywhere, Category = Stat, meta = (AllowPrivateAccess = "true"))
		float AD;

	/* 최대 기본 속도 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
		float MaximumWalkSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
		float MaximumSprintSpeed;
	

	/* 전투 모드인지 */
	UPROPERTY(VisibleAnywhere, Category = Combat, meta = (AllowPrivateAccess = "true"))
		bool bIsBattleMode;

	/* 죽었는지 */
	UPROPERTY(VisibleAnywhere, Category = Combat, meta = (AllowPrivateAccess = "true"))
	bool bDying;

	UPROPERTY(VisibleAnywhere, Category = Combat, meta = (AllowPrivateAccess = "true"))
	bool bSprinting;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	UParticleSystem* BloodParticle;

	/* 공격 시 TraceSphere를 시각화할지 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	bool bVisibleTraceSphere;


	/* Shield Variable */
	bool bIsShieldImpact;

	
	FVector LastHitDirection;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

private:

	bool ChangeSpeed;

	FName LeftFootSocketName = FName("Foot_LSocket");
	FName RightFootSocketName = FName("Foot_RSocket");

public:
	FORCEINLINE bool GetSprinting() const { return bSprinting; }
	FORCEINLINE bool GetAttacking() const { return CombatState == ECombatState::ECS_Attack; }
	FORCEINLINE bool GetGuarding() const { return CombatState == ECombatState::ECS_Guard; }
	FORCEINLINE bool GetImpacting() const { return CombatState == ECombatState::ECS_Impact; }
	FORCEINLINE bool GetShiledImpact() const { return bIsShieldImpact; }
	FORCEINLINE void SetShiledImpact(bool NextBool) { bIsShieldImpact = NextBool; }
	FORCEINLINE bool GetBattleMode() const { return bIsBattleMode; }
	FORCEINLINE bool GetDying() const { return bDying; }
	FORCEINLINE UParticleSystem* GetBloodParticle() const { return BloodParticle; }
	FORCEINLINE FVector GetLastHitDirection() const { return LastHitDirection; }
};
