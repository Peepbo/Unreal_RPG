// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Magic/Magic.h"
#include "LaserBeam.generated.h"

class UNiagaraComponent;
/**
 * 
 */
UCLASS()
class UNREALRPG_API ALaserBeam : public AMagic
{
	GENERATED_BODY()
	
public:
	ALaserBeam();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

protected:

private:
	/* Trace에 필요한 원의 크기 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = LaserBeam, meta = (AllowPrivateAccess = "true"))
	float TraceSphereRadius;

	/* Trace 최대 길이 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = LaserBeam, meta = (AllowPrivateAccess = "true"))
	float TraceMaximumDistance;

	/* FX 유지 시간 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = LaserBeam, meta = (AllowPrivateAccess = "true"))
	float NiagaraLifeTime;

	/* Trace 유지 시간 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = LaserBeam, meta = (AllowPrivateAccess = "true"))
	float TraceLifeTime;

	/* Trace 시작점 FX */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = LaserBeam, meta = (AllowPrivateAccess = "true"))
	UNiagaraComponent* StartPointFX;

	/* Trace 종료 FX */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = LaserBeam, meta = (AllowPrivateAccess = "true"))
	UNiagaraComponent* EndPointFX;


	FTimerHandle DestroyTimer;

	UPROPERTY(EditDefaultsOnly)
	bool bDrawDebugTrace;
};
