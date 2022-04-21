// Fill out your copyright notice in the Description page of Project Settings.


#include "MeleeCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Items/Item.h"
#include "Items/Weapon.h"
#include "Items/Shield.h"
#include "Engine/SkeletalMeshSocket.h"
#include "MeleeAnimInstance.h"
#include "Enemy/Enemy.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "..\Public\MeleeCharacter.h"

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
	ChangeSpeed(false),
	LastDamagedAttackType(EAttackType::EAT_Light)
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

void AMeleeCharacter::ChangeCombatState(ECombatState NextCombatState)
{
	CombatState = NextCombatState;
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

bool AMeleeCharacter::CustomApplyDamage(AActor* DamagedActor, float DamageAmount, AActor* DamageCauser, EAttackType AttackType)
{
	if (DamagedActor == nullptr || DamageCauser == nullptr)
	{
		return false;
	}

	return CustomTakeDamage(DamageAmount, DamageCauser, AttackType);
}

bool AMeleeCharacter::CustomTakeDamage(float DamageAmount, AActor* DamageCauser, EAttackType AttackType)
{
	if (bDying)
	{
		return false;
	}


	const FVector HitDir{ UKismetMathLibrary::Normal(DamageCauser->GetActorLocation() - GetActorLocation()) };
	const FVector ForwardVector{ GetActorForwardVector() };

	const float DotResult{ UKismetMathLibrary::DotProduct2D(FVector2D(ForwardVector), FVector2D(HitDir)) };
	float RelativeDegree{ UKismetMathLibrary::DegAcos(DotResult) };

	const float CrossResult{UKismetMathLibrary::CrossProduct2D(FVector2D(ForwardVector), FVector2D(HitDir)) };
	if (CrossResult < 0.f) 
	{
		RelativeDegree = -RelativeDegree;
	}
	UE_LOG(LogTemp, Warning, TEXT("Hit Degree : %f"), RelativeDegree);
	LastHitDirection = { UKismetMathLibrary::DegCos(RelativeDegree),UKismetMathLibrary::DegSin(RelativeDegree), 0.f };

	LastDamagedAttackType = AttackType;

	if (HP - DamageAmount > 0.f) {
		HP -= DamageAmount;
	}
	else {
		HP = 0.f;
		bDying = true;
	}

	return true;
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

FVector AMeleeCharacter::GetFootLocation(bool bLeft)
{
	return FVector();
}

//bool AMeleeCharacter::CheckFootCollision(bool bLeft, FVector& Out_HitPoint)
//{
//	const FName SelectSocketName{ bLeft ? LeftFootSocketName : RightFootSocketName };
//	
//	if (GetMesh()->DoesSocketExist(SelectSocketName)) 
//	{
//		const FVector StartLocation{ GetMesh()->GetSocketLocation(SelectSocketName) };
//		const FVector EndLocation{ StartLocation + FVector{0.f,0.f,-25.f} };
//
//		FHitResult HitResult;
//
//		bool bHit = UKismetSystemLibrary::LineTraceSingle(
//			this,
//			StartLocation,
//			EndLocation,
//			ETraceTypeQuery::TraceTypeQuery1,
//			false,
//			{ this },
//			EDrawDebugTrace::None,
//			HitResult,
//			true);
//
//		if (bHit) 
//		{
//			Out_HitPoint = HitResult.Location;
//		}
//
//		return bHit;
//	}
//	return false;
//}

void AMeleeCharacter::SaveRelativeVelocityAngle()
{
	float Angle{ 0.f };

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

	Angle = dot;
	if (Cross < 0.f) 
{
		Angle = -Angle;
	}
	
	UAnimInstance* AnimInst { GetMesh()->GetAnimInstance() };
	if (AnimInst) 
	{
		UMeleeAnimInstance* MeleeAnimInst{ Cast<UMeleeAnimInstance>(AnimInst) };
		if (MeleeAnimInst) 
		{
			MeleeAnimInst->SetLastRelativeVelocityAngle(Angle);
		}
	}
}
