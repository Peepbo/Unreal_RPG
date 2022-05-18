// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/DarkKnight/DarkKnight.h"
#include "Interface/Patrolable.h"
#include "PatrolDarkKnight.generated.h"

/**
 * 
 */
UCLASS()
class UNREALRPG_API APatrolDarkKnight : public ADarkKnight, public IPatrolable
{
	GENERATED_BODY()
	
public:
	// Sets default values for this character's properties
	APatrolDarkKnight();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	virtual void Tick(float DeltaTime) override;

private:
	UFUNCTION(CallInEditor, Category = "Patrol", meta = (AllowPrivateAccess = "true"))
	void CallRetargetPathAndDraw();

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Patrol", meta = (AllowPrivateAccess = "true", MakeEditWidget = "true"))
	TArray<FVector> PatrolPoints;

	/* Patrol Point를 World좌표로 바꾼 결과 */
	UPROPERTY()
	TArray<FVector> WorldPatrolPoints;

	/* 
	* ex) 0? 0->1로 이동하는 경로, last? last->0으로 이동하는 경로 
	* blueprint Construction에서 경로를 시각화하면서 정보를 여기로 보냄
	* 매번 같은곳으로 이동하는 정찰의 최적화를 하기 위함
	*/
	TArray<TArray<FVector>> PatrolPath;

	int32 PatrolIndex;

	bool bPatrol;

public:
	UFUNCTION(BlueprintCallable, Category = "Patrol")
	virtual int32 GetPatrolIndex() override;

	UFUNCTION(BlueprintCallable, Category = "Patrol")
	virtual TArray<FVector> GetPatrolPath() override;

	UFUNCTION(BlueprintCallable, Category = "Patrol")
	virtual void InitPatrolPath(int32 Size) override;

	UFUNCTION(BlueprintCallable, Category = "Patrol")
	virtual void DrawPatrolPath(float DrawTime = 0.f) override;

	UFUNCTION(BlueprintCallable, Category = "Patrol")
	virtual void RetargetPatrolPath() override;

	UFUNCTION(BlueprintCallable, Category = "Patrol")
	virtual	void NextPath() override;

	UFUNCTION(BlueprintCallable, Category = "Patrol")
	virtual FVector GetActorToPatrolFirstPointDirection() override;
};
