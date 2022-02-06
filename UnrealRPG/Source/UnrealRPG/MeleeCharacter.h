// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MeleeCharacter.generated.h"

UCLASS()
class UNREALRPG_API AMeleeCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMeleeCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/* 앞이나 뒤로 이동할 때 호출하는 함수 */
	void MoveForward(float Value);

	/* 오른쪽이나 왼쪽으로 이동할 때 호출하는 함수 */
	void MoveRight(float Value);

	/*
	* 주어진 속도로 카메라 좌우를 회전하는 함수
	* @param Rate	정규화된 값이며 1.f는 100%를 의미합니다.
	*/
	void TurnAtRate(float Rate);

	/*
	* 주어진 속도로 카메라 상하를 회전하는 함수
	* @param Rate	정규화된 값이며, 1.f는 100%를 의미합니다.
	*/
	void LookUpAtRate(float Rate);

	/* 공격 버튼이 눌렸을 때 작동하는 함수 */
	void Attack();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:

	/* 카메라를 달아 놓을 카메라 팔 추가 (카메라와 캐릭터 사이에 일정 거리를 두기위해 만든 컴포넌트) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/* 캐릭터를 따라 움직이는 카메라 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	/* 좌우를 둘러보는 감도 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float BaseTurnRate;

	/* 상하를 둘러보는 감도 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float BaseLookUpRate;

	/* 공격 몽타주 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class UAnimMontage* AttackMontage;
public:

	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
};
