// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"
#include "SavePoint.generated.h"

USTRUCT(BlueprintType)
struct FCheckPointTable : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName LevelName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector DefaultLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FVector> Location;
};

UCLASS()
class UNREALRPG_API ASavePoint : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASavePoint();

UFUNCTION(BlueprintImplementableEvent)
	void ClosePlayerEffect();

	void ChangeOverlapSetting(class APlayerCharacter* Player);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
		void PlayerRangeOverlap(
			UPrimitiveComponent* OverlappedComponent,
			AActor* OtherActor,
			UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex,
			bool bFromSweep,
			const FHitResult& SweepResult);

	UFUNCTION()
		void PlayerRangeEndOverlap(
			UPrimitiveComponent* OverlappedComponent,
			AActor* OtherActor,
			UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex);



private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = SavePoint, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* DecolationPlane;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = SavePoint, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* DecolationMesh1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = SavePoint, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* DecolationMesh2;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = SavePoint, meta = (AllowPrivateAccess = "true"))
	class UNiagaraComponent* FX;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = SavePoint, meta = (AllowPrivateAccess = "true"))
	class USpotLightComponent* SpotLight;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = SavePoint, meta = (AllowPrivateAccess = "true"))
	class UPointLightComponent* PointLight;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = SavePoint, meta = (AllowPrivateAccess = "true"))
	class UAudioComponent* Audio;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = SavePoint, meta = (AllowPrivateAccess = "true"))
	class USphereComponent* OverlapSphere;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = SavePoint, meta = (AllowPrivateAccess = "true"))
	class USceneComponent* ResponPoint;

	class APlayerCharacter* PlayerCharacter;

	//UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = DataTable, meta = (AllowPrivateAccess = "true"))
	//	UDataTable* DataTable;

	bool bClosePlayer;

	UPROPERTY(EditAnywhere, Category = SavePoint, meta = (AllowPrivateAccess = "true"))
	FName SavePointName;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	FORCEINLINE FName GetSavePointName() const { return SavePointName; }
	FORCEINLINE FTransform GetResponPointTransform() const { return ResponPoint->GetComponentTransform(); }
};
