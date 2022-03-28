// Fill out your copyright notice in the Description page of Project Settings.


#include "MeleeAnimInstance.h"
#include "MeleeCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void UMeleeAnimInstance::InitializeAnimationProperties()
{
	Character = Cast<AMeleeCharacter>(TryGetPawnOwner());

	LastRelativeVelocityAngle = 0.f;
}

void UMeleeAnimInstance::UpdateAnimationProperties(float DeltaTime)
{
	if (Character == nullptr) {
		Character = Cast<AMeleeCharacter>(TryGetPawnOwner());
	}

	if (Character) {
		// 캐릭터의 속도를 구함
		FVector CharVelocity{ Character->GetVelocity() };
		CharVelocity.Z = 0;
		Velocity = { CharVelocity.X,CharVelocity.Y };

		if (!Character->GetAttacking()) {
			AttackExclusionVelocity = Velocity;
			AttackExclusionSpeed = AttackExclusionVelocity.Size();
		}

		// 일정 속도 이상이여야 Brake 애니메이션을 호출하기 위해 속도 최소값(200.f)을 지정함
		if (Velocity.Size() > 200.f) {
			LastRelativeVelocityAngle = Character->GetLastRelativeVelocityAngle();
		}

		Speed = Velocity.Size();

		// 캐릭터가 공중에 있는지 확인
		bIsInAir = Character->GetCharacterMovement()->IsFalling();

		// 캐릭터가 움직이고 있는지 확인
		if (Character->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0) {
			bIsAccelerating = true;
		}
		else {
			bIsAccelerating = false;
		}

		bIsSprint = Character->GetSprinting();

		bAttack = Character->GetAttacking();

		bDying = Character->GetDying();

		bIsShieldImpact = Character->GetShiledImpact();

		bIsDamageImpact = Character->GetImpacting();

		LastHitDirection = Character->GetLastHitDirection();
	}
}
