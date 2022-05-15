// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Magic.generated.h"

/* Magic에 사용하는 NiagaraFX에 꼭 포함되어야 할 Param
* 1. Fire (특정 조건에 만족하여 종료할 때 천천히 파티클 생성을 줄일 때 사용하는 Param)
* 2. Size (FX의 크기를 다르게하고싶을 때 사용하는 Param)
*/
UCLASS()
class UNREALRPG_API AMagic : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMagic();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void OnConstruction(const FTransform& Transform) override;

	bool CastAble() const { return (FX && Mesh); }

	void PlayCollisionSound();

public:	
	// Called every frame1
	virtual void Tick(float DeltaTime) override;

	void InitMagic(APawn* CastOwner, float Damage);

	virtual void ActiveMagic(float DeltaTime);
	virtual void DeactiveMagic(float DeltaTime);

	void SetMeshVisiblilty(bool NextVisible);

protected:
	const FName FireParam = FName("Fire");
	const FName SizeParam = FName("Size");

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Magic)
	float MagicDamage;
	/* 마법을 시젼하는 주인 */
	APawn* MagicOwner;

	bool bEndFXParticle;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Magic)
		class UParticleSystem* EndFX_particle;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Magic)
		class UNiagaraSystem* EndFX_niagara;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Magic)
		class USoundCue* FXSound;
		
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Magic)
		USoundCue* CollisionSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Magic)
		class USoundAttenuation* SoundAttenuation;


	UPROPERTY(EditDefaultsOnly, Category = "FX Param")
		float FXSize;

	UPROPERTY(EditDefaultsOnly, Category = "FX Param")
		float FXSpawnRate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Magic)
	class UNiagaraComponent* FX;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Magic)
	class UStaticMeshComponent* Mesh;

	bool bUseStaticMesh;

	bool bStop;

private:


public:
};
