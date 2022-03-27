// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerAnimInstance.h"
#include "PlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void UPlayerAnimInstance::InitializeAnimationProperties()
{
	Super::InitializeAnimationProperties();

	PlayerCharacter = Cast<APlayerCharacter>(TryGetPawnOwner());
	LastFootCurveValue = 0.f;
	CurveName = TEXT("WalkStop");
	bIsRightFootStop = true;
}

void UPlayerAnimInstance::UpdateAnimationProperties(float DeltaTime)
{
	// UMeleeAnimInstance에서 캐릭터의 공통 정보를 업데이트 함 (속도, 움직이는지, 공중인지)
	Super::UpdateAnimationProperties(DeltaTime);

	if (PlayerCharacter == nullptr)
	{
		PlayerCharacter = Cast<APlayerCharacter>(TryGetPawnOwner());
	}

	if (PlayerCharacter)
	{
		// 스프린트 상태인지 확인한다.
		//bIsSprint = MeleeCharacter->GetSprinting();

		// 가드 상태인지 확인한다.
		bIsGuard = PlayerCharacter->GetGuarding();

		// 전투 모드 상태인지 확인한다.
		bIsBattleMode = PlayerCharacter->GetBattleMode();

		// 락온 상태인지 확인한다.
		bLockOn = PlayerCharacter->GetLockOn();

		// 캐릭터의 움직임 방향을 읽어온다.
		MoveValue = PlayerCharacter->GetThumStickAxisForce();

		LastMoveValue = PlayerCharacter->GetLastMoveValue();

		PlayerForward = PlayerCharacter->GetActorForwardVector();

		bIsMove = (MoveValue.Size() != 0.f);


		bRoll = (PlayerCharacter->GetCombatState() == ECombatState::ECS_Roll);

		LastRollMoveValue = PlayerCharacter->GetLastRollMoveValue();

		bBackDodge = PlayerCharacter->GetBackDodge();

		LastFootCurveValue = GetCurveValue(CurveName);

		bDrinkingPotion = PlayerCharacter->GetDrinking();

	}
}

void UPlayerAnimInstance::ChooseStopFoot()
{
	const bool bIsLeft{ UKismetMathLibrary::InRange_FloatFloat(LastFootCurveValue, 1.f, 2.f) };
	bIsRightFootStop = !bIsLeft;
}
