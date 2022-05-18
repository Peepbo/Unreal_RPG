// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Magic.generated.h"

class UParticleSystem;
class UNiagaraSystem;
class USoundCue;
class USoundAttenuation;
class UNiagaraComponent;
class UStaticMeshComponent;

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

	bool CastAble() const { return (FX != nullptr && Mesh != nullptr); }

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
	UPROPERTY()
	APawn* MagicOwner;

	bool bEndFXParticle;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Magic)
	UParticleSystem* EndFX_particle;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Magic)
	UNiagaraSystem* EndFX_niagara;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Magic)
	USoundCue* FXSound;
		
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Magic)
	USoundCue* CollisionSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Magic)
	USoundAttenuation* SoundAttenuation;


	UPROPERTY(EditDefaultsOnly, Category = "FX Param")
	float FXSize;

	UPROPERTY(EditDefaultsOnly, Category = "FX Param")
	float FXSpawnRate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Magic)
	UNiagaraComponent* FX;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Magic)
	UStaticMeshComponent* Mesh;

	bool bUseStaticMesh;

	bool bStop;

private:


public:
};
