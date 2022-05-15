// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Magic.h"
#include "ProjectileMagic.generated.h"

/**
 * 
 */
UCLASS()
class UNREALRPG_API AProjectileMagic : public AMagic
{
	GENERATED_BODY()
public:
	AProjectileMagic();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	virtual void ActiveMagic(float DeltaTime) override;
	virtual void DeactiveMagic(float DeltaTime) override;

private:
	float FXSpeed;
	UPROPERTY(EditDefaultsOnly, Category = Setting, meta = (AllowPrivateAccess = "true"))
	float MaximumFXSpeed;

	/* Lerp속도인지 Interp속도인지 구분한다 */
	bool bUseLerp;

	/* Projectile이 종료 시 바로 제거할지, 모든 오브젝트가 삭제됬을 때 제거할지 구분한다 */
	UPROPERTY(EditDefaultsOnly, Category = "Fade Out", meta = (AllowPrivateAccess = "true"))
	bool bAutoFadeOut;

	UPROPERTY(EditDefaultsOnly, Category = "Fade Out", meta = (AllowPrivateAccess = "true"))
	float SpawnRateFadeOutSpeed;
};
