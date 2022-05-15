// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/Enemy.h"
#include "DarkKnight.generated.h"

/**
 * 
 */

UCLASS()
class UNREALRPG_API ADarkKnight : public AEnemy
{
	GENERATED_BODY()
	
public:
	ADarkKnight();

protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void SetBackWeaponVisibility(const bool bNextVisibility);

	UFUNCTION(BlueprintCallable)
	void SetEquipWeaponVisibility(const bool bNextVisibility);

	virtual void AgroSphereOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult) override;

	virtual void AgroSphereEndOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex) override;

	UFUNCTION(BlueprintCallable)
	void StartDraw();

	UFUNCTION(BlueprintCallable)
	void EndDraw();

	UFUNCTION(BlueprintCallable)
	void StartSheath();

	UFUNCTION(BlueprintCallable)
	void EndSheath();

	virtual void EndDamageImpact() override;

	virtual	void PlayAttackMontage() override;

	virtual void FindCharacter() override;

private:
	virtual void ResetCombat() override;

	void DropWeapon();

	/* 시작 전에 나이트의 배틀 모드 상태를 바꾸는 버튼(함수) */
	UFUNCTION(CallInEditor, Category = "Setting", meta = (AllowPrivateAccess = "true"))
	void ChangeBeginBattleMode();

private:
	class UKnightAnimInstance* KnightAnimInstance;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class USkeletalMeshComponent* WeaponCaseMesh;

	UPROPERTY(EditDefaultsOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* DrawMontage;

	UPROPERTY(EditDefaultsOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* SheathMontage;
	
	UPROPERTY(VisibleAnywhere, Category = Temp, meta = (AllowPrivateAccess = "true"))
	bool bShouldDrawWeapon;

	UPROPERTY(VisibleAnywhere, Category = Temp, meta = (AllowPrivateAccess = "true"))
		FVector EnemyDir;
	UPROPERTY(VisibleAnywhere, Category = Temp, meta = (AllowPrivateAccess = "true"))
		FVector EnemyToTargetDir;

	FTimerHandle WalkDirectionLerpTimer;

	UPROPERTY(EditDefaultsOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float DirectionLerpSpeed;

	UPROPERTY(EditDefaultsOnly, Category = Rotate, meta = (AllowPrivateAccess = "true"))
	float TurnTime;

	UPROPERTY(EditDefaultsOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	FEnemyNormalAttack SprintAttack;

	FTimerHandle DropWeaponTimer;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual bool CustomTakeDamage(float DamageAmount, AActor* DamageCauser, EAttackType AttackType) override;

public:
	FORCEINLINE bool ShouldDrawWeapon() const { return bShouldDrawWeapon; }
};
