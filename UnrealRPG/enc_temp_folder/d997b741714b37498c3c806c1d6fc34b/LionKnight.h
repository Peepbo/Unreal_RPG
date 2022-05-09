// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/Enemy.h"
#include "LionKnight.generated.h"

class UStaticMeshComponent;
class APlayerCharacter;
/**
 * 
 */
UCLASS()
class UNREALRPG_API ALionKnight : public AEnemy
{
	GENERATED_BODY()
	
public:
	ALionKnight();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual void ResetCombat() override;

	virtual void EndAttack(bool bChooseNextAttack = true) override;

	UFUNCTION(BlueprintCallable)
	void UseMagic();

	UFUNCTION(BlueprintCallable)
	void ReadyToBattle();

	UFUNCTION(BlueprintCallable)
	void GrapWeapon();

	void ResetBoss();

	UFUNCTION(BlueprintCallable)
	void EndDodge();

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Magic", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class AProjectileMagic> ProjectileMagic;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Magic", meta = (AllowPrivateAccess = "true"))
	float ProjectileDamage;

	bool bPrepareBattle;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Setting", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* GroundedWeapon;

	FTimerHandle ChangeWeaponSettingTimer;

	/* 3방향 회피  0? 왼쪽, 1? 오른쪽, 2? 후면 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	TArray<FEnemyAdvancedAttack> DodgeMontage;

	int32 DodgeWaitCount;
	int32 MaximumDodgeWaitCount;

public:
	FORCEINLINE bool ShouldPrepareBattle() const { return bPrepareBattle; }
};
