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
	//virtual void ChangeBattleMode() override;

private:

	class UKnightAnimInstance* AnimInstance;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	TArray<UAnimMontage*> AttackMontage;

	UPROPERTY(EditDefaultsOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* DrawMontage;
	
	bool bShouldDrawWeapon;
	// spd 130/400

	UPROPERTY(VisibleAnywhere, Category = Temp, meta = (AllowPrivateAccess = "true"))
		FVector EnemyDir;
	UPROPERTY(VisibleAnywhere, Category = Temp, meta = (AllowPrivateAccess = "true"))
		FVector EnemyToTargetDir;


public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
