// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/Enemy.h"
#include "LionKnight.generated.h"

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

	UFUNCTION(BlueprintCallable)
	void UseMagic();

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Magic", meta = (AllowPrivateAccess = "true"))
		TSubclassOf<class AProjectileMagic> ProjectileMagic;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Magic", meta = (AllowPrivateAccess = "true"))
		float ProjectileDamage;

private:
	virtual	void PlayAttackMontage() override;
};
