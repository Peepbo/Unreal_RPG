// Fill out your copyright notice in the Description page of Project Settings.


#include "MeleeCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Item.h"
#include "Weapon.h"
#include "Shield.h"
#include "Engine/SkeletalMeshSocket.h"
#include "MeleeAnimInstance.h"
#include "Enemy.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
AMeleeCharacter::AMeleeCharacter() :
	HP(100.f),
	MaximumHP(100.f),
	AD(0.f),
	MaximumWalkSpeed(500.f),
	MaximumSprintSpeed(800.f),
	bDying(false),
	CombatState(ECombatState::ECS_Unoccupied),
	bVisibleTraceSphere(false),
	bIsShieldImpact(false),
	LastHitDirection(0.f,0.f,0.f),
	ChangeSpeed(false)
{
	GetCharacterMovement()->RotationRate = FRotator(0.f, 1000.f, 0.f);
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;
	GetCharacterMovement()->MaxWalkSpeed = MaximumWalkSpeed;
}

// Called when the game starts or when spawned
void AMeleeCharacter::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AMeleeCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//if (bSprinting && GetCharacterMovement()->GetCurrentAcceleration().IsNearlyZero()) {
	//	HardResetSprint();
	//}
	ChangeMaximumSpeedForSmoothSpeed(DeltaTime);
}

float AMeleeCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (bDying)return DamageAmount;

	const FVector HitDir{ DamageCauser->GetActorLocation() - GetActorLocation() };
	LastHitDirection = UKismetMathLibrary::Normal(HitDir);

	if (HP - DamageAmount > 0.f) {
		HP -= DamageAmount;
	}
	else {
		HP = 0.f;
		bDying = true;
	}

	return DamageAmount;
}

float AMeleeCharacter::GetLastRelativeVelocityAngle()
{
	float ReturnAngle{ 0.f };

	const FVector Forward{ GetActorForwardVector() };
	const FVector Velocity{ GetCharacterMovement()->Velocity };

	const FVector2D NormalVelocity{ UKismetMathLibrary::Normal2D({Velocity.X,Velocity.Y}) };
	const FVector2D Forward2D{ Forward.X,Forward.Y };

	// dot 후 radian값을 degree로 변환
	const float dot{ UKismetMathLibrary::DegAcos(
		UKismetMathLibrary::DotProduct2D(
			Forward2D,
			NormalVelocity)) };

	// 각도가 음수인지 양수인지 판별하기 위해 cross 시도
	const float Cross{ UKismetMathLibrary::CrossProduct2D(Forward2D, NormalVelocity) };

	ReturnAngle = dot;
	if (Cross < 0.f) {
		ReturnAngle = -ReturnAngle;
	}

	return ReturnAngle;
}

void AMeleeCharacter::EndShieldImpact()
{
	bIsShieldImpact = false;
}

void AMeleeCharacter::EndDamageImpact()
{
	CombatState = ECombatState::ECS_Unoccupied;
}

void AMeleeCharacter::ChangeMoveState(bool bNextMoveSprinting)
{
	bSprinting = bNextMoveSprinting;
	ChangeSpeed = true;
}

void AMeleeCharacter::HardResetSprint()
{
	bSprinting = false;
	GetCharacterMovement()->MaxWalkSpeed = MaximumWalkSpeed;
}

void AMeleeCharacter::ChangeMaximumSpeedForSmoothSpeed(float DeltaTime)
{
	if (ChangeSpeed) {
		const float TargetSpeed{ (bSprinting ? MaximumSprintSpeed : MaximumWalkSpeed) };
		float NowSpeed{ GetCharacterMovement()->MaxWalkSpeed };

		NowSpeed = UKismetMathLibrary::FInterpTo(NowSpeed, TargetSpeed, DeltaTime, 10.f);

		if (UKismetMathLibrary::NearlyEqual_FloatFloat(NowSpeed, TargetSpeed)) {
			NowSpeed = TargetSpeed;
			ChangeSpeed = false;
		}

		GetCharacterMovement()->MaxWalkSpeed = NowSpeed;
	}
}