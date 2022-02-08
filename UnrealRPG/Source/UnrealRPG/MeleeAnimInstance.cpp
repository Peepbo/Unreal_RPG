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
	}
	if (MeleeCharacter) {
		// 실질적으로 바라보고있는 방향을 가져온다.
		const FRotator BaseAimRot = MeleeCharacter->GetBaseAimRotation();
		// 캐릭터의 방향을 받아온다.
		const FRotator ActorRotation = MeleeCharacter->GetActorRotation();

		// 캐릭터의 머리가 봐야할 각도를 구한다.
		const FRotator Result = UKismetMathLibrary::NormalizedDeltaRotator(BaseAimRot, ActorRotation);

		// PitchOfLook, YawOfLook 둘 다 업데이트 해준다.
		PitchOfLook = Result.Pitch;
		YawOfLook = Result.Yaw;
	}
}
