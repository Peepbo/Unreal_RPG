// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EventArea.h"
#include "ExecutionArea.generated.h"

class AEnemy;
/**
 * 
 */
UCLASS()
class UNREALRPG_API AExecutionArea : public AEventArea
{
	GENERATED_BODY()
	
public:
	AExecutionArea();

public:
	void ActiveExecution();

	void InitExecutionData(APlayerCharacter* Character, float FDamage, float SDamage);
	
private:
	UFUNCTION(BlueprintCallable)
	void ApplyDamageToEnemy(int32 Index);

	virtual void InitEventLocationAndRotation() override;

protected:
	virtual	void PlayerRangeOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult) override;

protected:
	AEnemy* Enemy;
	bool bActive;

private:
	TArray<float> ExecutionDamages;


public:
	FORCEINLINE AEnemy* GetEnemy() const { return Enemy; }
	FORCEINLINE void SetEnemy(AEnemy* Enem) { Enemy = Enem; }
};
