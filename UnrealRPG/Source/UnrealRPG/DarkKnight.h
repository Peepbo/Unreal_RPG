// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy.h"
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

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void ShowBackWeapon();
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void HideBackWeapon();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void ShowWeapon();
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void HideWeapon();

	virtual void AgroSphereOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult) override;

	UFUNCTION(BlueprintCallable)
	void StartDraw();

	UFUNCTION(BlueprintCallable)
	void PlayAttackMontage();

	UFUNCTION(BlueprintCallable)
	void SaveTargetRotator();

	UFUNCTION(BlueprintCallable)
	void StartRotate();

	UFUNCTION(BlueprintCallable)
	void StopRotate();

	UFUNCTION(BlueprintCallable)
	void GetWeaponMesh(class USkeletalMeshComponent* ItemMesh);

	void TracingAttackSphere();

	UFUNCTION(BlueprintCallable)
	void StartAttackCheckTime();

	UFUNCTION(BlueprintCallable)
	void EndAttackCheckTime();

	UFUNCTION(BlueprintCallable)
	void ChangeCombatState(ECombatState NextCombatState);

	UFUNCTION(BlueprintCallable)
	void FaceOff(int32 NextWalkDirection);

	UFUNCTION(BlueprintCallable)
	void EndFaceOff();

	void ChangeLerpDirection();

	UFUNCTION(BlueprintCallable)
	void StartRestTimer();

	void EndRestTimer();

private:

	class UKnightAnimInstance* AnimInstance;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	TArray<UAnimMontage*> AttackMontage;

	UPROPERTY(EditDefaultsOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* DrawMontage;
	
	bool bShouldDrawWeapon;
	// spd 130/400
	// atk 20

	UPROPERTY(VisibleAnywhere, Category = Temp, meta = (AllowPrivateAccess = "true"))
		FVector EnemyDir;
	UPROPERTY(VisibleAnywhere, Category = Temp, meta = (AllowPrivateAccess = "true"))
		FVector EnemyToTargetDir;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	FRotator LastSaveRotate;

	int32 AttackIndex;
	int32 LastAttackIndex;

	bool bIsRotate;

	UPROPERTY(EditDefaultsOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float InterpSpeed;

	UPROPERTY(VisibleAnywhere, Category = Combat, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* WeaponMesh;

	FTimerHandle AttackCheckTimer;

	bool bAttackable;

	/* -1 : left, 0 : forward, 1 : right */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float WalkDirection;

	FTimerHandle WalkDirectionLerpTimer;

	UPROPERTY(EditDefaultsOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float DirectionLerpSpeed;

	FTimerHandle RestTimer;

	UPROPERTY(EditDefaultsOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	bool bVisibleTraceSphere;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	FORCEINLINE float GetWalkDirection() const { return WalkDirection; }

};
