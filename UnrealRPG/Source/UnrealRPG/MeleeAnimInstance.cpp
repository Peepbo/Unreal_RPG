// Fill out your copyright notice in the Description page of Project Settings.


#include "MeleeAnimInstance.h"
#include "MeleeCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

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
}
