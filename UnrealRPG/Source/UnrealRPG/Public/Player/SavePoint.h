// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EventArea.h"
#include "Engine/DataTable.h"
#include "SavePoint.generated.h"

USTRUCT(BlueprintType)
struct FCheckPointTable : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector DefaultLocation;
};

class UStaticMeshComponent;
class UNiagaraComponent;
class USpotLightComponent;
class UPointLightComponent;
class UAudioComponent;
class USceneComponent;

UCLASS()
class UNREALRPG_API ASavePoint : public AEventArea
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASavePoint();

protected:
	// Called when the game starts or when spawned
	virtual	void PlayerRangeOverlap(
			UPrimitiveComponent* OverlappedComponent,
			AActor* OtherActor,
			UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex,
			bool bFromSweep,
			const FHitResult& SweepResult) override;

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = SavePoint, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* DecolationPlane;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = SavePoint, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* DecolationMesh2;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = SavePoint, meta = (AllowPrivateAccess = "true"))
	UNiagaraComponent* FX;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = SavePoint, meta = (AllowPrivateAccess = "true"))
	USpotLightComponent* SpotLight;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = SavePoint, meta = (AllowPrivateAccess = "true"))
	UPointLightComponent* PointLight;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = SavePoint, meta = (AllowPrivateAccess = "true"))
	UAudioComponent* Audio;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = SavePoint, meta = (AllowPrivateAccess = "true"))
	USceneComponent* ResponPoint;

	UPROPERTY(EditAnywhere, Category = SavePoint, meta = (AllowPrivateAccess = "true"))
	FName SavePointName;

public:
	FORCEINLINE FName GetSavePointName() const { return SavePointName; }
	FORCEINLINE FTransform GetResponPointTransform() const { return ResponPoint->GetComponentTransform(); }
};
