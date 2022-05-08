// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EventArea.generated.h"

class UStaticMeshComponent;
class USphereComponent;
class APlayerCharacter;

UCLASS()
class UNREALRPG_API AEventArea : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEventArea();

	void ActiveAutoArrange(float Dist = 0.f);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UFUNCTION()
	virtual	void PlayerRangeOverlap(
			UPrimitiveComponent* OverlappedComponent,
			AActor* OtherActor,
			UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex,
			bool bFromSweep,
			const FHitResult& SweepResult);

	UFUNCTION()
	virtual	void PlayerRangeEndOverlap(
			UPrimitiveComponent* OverlappedComponent,
			AActor* OtherActor,
			UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex);

	UFUNCTION(BlueprintCallable)
	virtual bool MoveTargetEventLocation();

	virtual void InitEventLocationAndRotation();

protected:
	APlayerCharacter* PlayerCharacter;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = SavePoint, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* DecolationMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = EventArea, meta = (AllowPrivateAccess = "true"))
	USphereComponent* OverlapSphere;

	bool bClosePlayer;

	/* 이벤트 위젯에 띄울 설명 텍스트 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = EventArea, meta = (AllowPrivateAccess = "true"))
	FName EventText;

	FVector EventLocation;
	FRotator EventRotator;

private:
	bool bActiveAutoArrange;
	float Distance;

public:
	virtual void Tick(float DeltaTime) override;

	FORCEINLINE FName GetEventText() const { return EventText; }
};
