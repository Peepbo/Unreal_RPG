// Fill out your copyright notice in the Description page of Project Settings.


#include "MeleeAnimInstance.h"
#include "MeleeCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void UMeleeAnimInstance::InitializeAnimationProperties()
{
	Character = Cast<AMeleeCharacter>(TryGetPawnOwner());
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
		if (Velocity.Size() > 200.f) {
			FVector Forward{ Character->GetActorForwardVector() };
			const FVector2D NormalVelocity{ UKismetMathLibrary::Normal2D(Velocity) };
			const FVector2D Forward2D{ Forward.X,Forward.Y };
			const float dot{ UKismetMathLibrary::DegAcos(
				UKismetMathLibrary::DotProduct2D(
					Forward2D,
					NormalVelocity)) };
			const float Cross{ UKismetMathLibrary::CrossProduct2D(Forward2D, NormalVelocity) };
			
			LastRelativeVelocityAngle = dot;
			if (Cross < 0.f) {
				LastRelativeVelocityAngle = -LastRelativeVelocityAngle;
			}
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
