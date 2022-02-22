// Fill out your copyright notice in the Description page of Project Settings.


#include "MeleeAnimInstance.h"
#include "MeleeCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void UMeleeAnimInstance::NativeInitializeAnimation()
{
	MeleeCharacter = Cast<AMeleeCharacter>(TryGetPawnOwner());
}

void UMeleeAnimInstance::UpdateAnimationProperties(float DeltaTime)
{
	if (MeleeCharacter == nullptr) 
	{
		MeleeCharacter = Cast<AMeleeCharacter>(TryGetPawnOwner());
	}

	if (MeleeCharacter) 
	{
		// 캐릭터의 속도를 구함
		FVector Velocity{ MeleeCharacter->GetVelocity() };
		// Z(상하) 속도는 무시
		Velocity.Z = 0;
		Speed = Velocity.Size();

		// 캐릭터가 공중에 있는지 확인
		bIsInAir = MeleeCharacter->GetCharacterMovement()->IsFalling();

		// 캐릭터가 움직이고 있는지 확인
		if (MeleeCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0)
		{
			bIsAccelerating = true;
		}
		else
		{
			bIsAccelerating = false;
		}


		// 실질적으로 바라보고있는 방향을 가져온다.
		const FRotator BaseAimRot = MeleeCharacter->GetBaseAimRotation();
		// 캐릭터의 방향을 받아온다.
		const FRotator ActorRotation = MeleeCharacter->GetActorRotation();

		// 캐릭터의 머리가 봐야할 각도를 구한다.
		const FRotator Result = UKismetMathLibrary::NormalizedDeltaRotator(BaseAimRot, ActorRotation);

		// PitchOfLook, YawOfLook 둘 다 업데이트 해준다.
		PitchOfLook = Result.Pitch;
		YawOfLook = Result.Yaw;


		// 가만히 멈춰서 공격하는지 확인한다.
		const bool bIsAttack{ MeleeCharacter->GetAttacking() };
		// 공격중이며, 공중이 아니고, 움직이지 않는 경우가 True
		bIsAttackWithoutMoving = bIsAttack && !bIsInAir && Speed == 0;


		// 스프린트 상태인지 확인한다.
		bIsSprint = MeleeCharacter->GetSprinting();


		// 가드 상태인지 확인한다.
		bIsGuard = MeleeCharacter->GetGuarding();
	}
}
