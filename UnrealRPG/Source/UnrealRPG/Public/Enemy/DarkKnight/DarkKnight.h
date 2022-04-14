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

	virtual void EndDamageImpact() override;

	virtual	void PlayAttackMontage() override;

	virtual void FindCharacter() override;

private:
	void DropWeapon();

private:
	class UKnightAnimInstance* KnightAnimInstance;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class USkeletalMeshComponent* WeaponCaseMesh;

	UPROPERTY(EditDefaultsOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* DrawMontage;
	
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
	FEnemyAdvancedAttack SprintAttack;
	//	UAnimMontage* SprintAttackMontage;

	FTimerHandle DropWeaponTimer;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual bool CustomTakeDamage(float DamageAmount, AActor* DamageCauser, EAttackType AttackType) override;

public:
	FORCEINLINE bool ShouldDrawWeapon() const { return bShouldDrawWeapon; }
};
