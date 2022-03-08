// Fill out your copyright notice in the Description page of Project Settings.


#include "DarkKnight.h"
#include "KnightAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "PlayerCharacter.h"
#include "EnemyAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"

ADarkKnight::ADarkKnight() :
	bShouldDrawWeapon(true),
	AttackIndex(0),
	LastAttackIndex(0),
	bIsRotate(false),
	InterpSpeed(5.f),
	bAttackable(true),
	WalkDirection(0.f),
	DirectionLerpSpeed(1.f)
{

}

void ADarkKnight::BeginPlay()
{
	Super::BeginPlay();

	BattleWalkSpeed = 130.f;
	BattleRunSpeed = 400.f;
	MaximumWalkSpeed = 170.f;
	AD = 20.f;

	GetCharacterMovement()->MaxWalkSpeed = MaximumWalkSpeed;

	UAnimInstance* AnimInst = GetMesh()->GetAnimInstance();
	if (AnimInst) {
		UKnightAnimInstance* KnightAnimInst = Cast<UKnightAnimInstance>(AnimInst);

		if (KnightAnimInst) {
			AnimInstance = KnightAnimInst;
		}
	}

	AttackIndex = FMath::RandRange(0, AttackMontage.Num() - 1);

	ChangeEnemySize(EEnemySize::EES_Medium);
}

void ADarkKnight::AgroSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!bShouldDrawWeapon)return;
	if (bIsBattleMode)return;
	if (OtherActor == nullptr)return;

	auto Character = Cast<APlayerCharacter>(OtherActor);
	if (Character) {
		bShouldDrawWeapon = false;

		if (EnemyAIController) {
			Target = Character;
			EnemyAIController->GetBlackboardComponent()->SetValueAsObject(TEXT("Target"), Character);

			StartDraw();
		}
	}

}

void ADarkKnight::StartDraw()
{
	GetCharacterMovement()->StopActiveMovement();

	if (DrawMontage) {
		if (AnimInstance) {
			AnimInstance->Montage_Play(DrawMontage);
			UE_LOG(LogTemp, Warning, TEXT("Montage Play"));
		}
	}
}

void ADarkKnight::PlayAttackMontage()
{
	if (AnimInstance) {
		if (AttackMontage.IsValidIndex(AttackIndex) && AttackMontage[AttackIndex]) {
			AnimInstance->Montage_Play(AttackMontage[AttackIndex]);
			LastAttackIndex = AttackIndex;

			UE_LOG(LogTemp, Warning, TEXT("Play Attack Montage"));
		}
	}

	AttackIndex++;
	if (AttackMontage.Num() == AttackIndex) {
		AttackIndex = 0;
	}
}

void ADarkKnight::SaveTargetRotator()
{
	FVector TargetLoc{ Target->GetActorLocation() };
	FVector ActorLoc{ GetActorLocation() };
	TargetLoc.Z = ActorLoc.Z = 0.f;

	const FVector DirectionToTarget{ TargetLoc - ActorLoc };
	const FVector DirectionToTargetNormal{ UKismetMathLibrary::Normal(DirectionToTarget) };
	const float RotateDegree = UKismetMathLibrary::DegAtan2(DirectionToTargetNormal.Y, DirectionToTargetNormal.X);

	const FRotator RotationToTarget{ 0,RotateDegree,0 };
	LastSaveRotate = RotationToTarget;
}

void ADarkKnight::StartRotate()
{
	bIsRotate = true;
}

void ADarkKnight::StopRotate()
{
	bIsRotate = false;
}

void ADarkKnight::GetWeaponMesh(USkeletalMeshComponent* ItemMesh)
{
	if (ItemMesh) {
		WeaponMesh = ItemMesh;
	}
}

void ADarkKnight::TracingAttackSphere()
{
	if (!bAttackable)return;

	/* 아이템의 TopSocket과 BottomSocket의 위치를 받아온다. */
	const FVector TopSocketLoc{ WeaponMesh->GetSocketLocation("TopSocket") };
	const FVector BottomSocketLoc{ WeaponMesh->GetSocketLocation("BottomSocket") };

	FHitResult HitResult;
	// SphereTraceSingle로 원을 그리고 원에 겹치는 오브젝트를 HitResult에 담는다.
	bool bHit = UKismetSystemLibrary::SphereTraceSingle(
		this,
		BottomSocketLoc,
		TopSocketLoc,
		50.f,
		ETraceTypeQuery::TraceTypeQuery1,
		false,
		{ this },
		//EDrawDebugTrace::ForDuration,
		EDrawDebugTrace::None,
		HitResult,
		true
	);

	if (bHit) {
		if (HitResult.Actor != nullptr) {
			auto Player = Cast<APlayerCharacter>(HitResult.Actor);
	
			if (Player) {
				UGameplayStatics::ApplyDamage(
					Player,
					AD,
					GetController(),
					this,
					UDamageType::StaticClass());

				bAttackable = false;
			}
		}
	}
}

void ADarkKnight::StartAttackCheckTime()
{
	GetWorldTimerManager().SetTimer(
		AttackCheckTimer,
		this,
		&ADarkKnight::TracingAttackSphere,
		0.005f,
		true);
}

void ADarkKnight::EndAttackCheckTime()
{
	GetWorldTimerManager().ClearTimer(AttackCheckTimer);

	bAttackable = true;
}

void ADarkKnight::ChangeCombatState(ECombatState NextCombatState)
{
	CombatState = NextCombatState;
}

void ADarkKnight::FaceOff(int32 NextWalkDirection)
{
	GetCharacterMovement()->bUseControllerDesiredRotation = false;

	WalkDirection = NextWalkDirection;
}

void ADarkKnight::EndFaceOff()
{
	GetCharacterMovement()->bUseControllerDesiredRotation = true;

	// Start Timer
	GetWorldTimerManager().SetTimer(
		WalkDirectionLerpTimer,
		this,
		&ADarkKnight::ChangeLerpDirection,
		0.005f,
		true);
}

void ADarkKnight::ChangeLerpDirection()
{
	if (UKismetMathLibrary::EqualEqual_FloatFloat(WalkDirection, 0.f)) {
		WalkDirection = 0.f;
		GetWorldTimerManager().ClearTimer(WalkDirectionLerpTimer);
		return;
	}

	WalkDirection = UKismetMathLibrary::Lerp(WalkDirection, 0.f, DirectionLerpSpeed);
}

void ADarkKnight::StartRestTimer()
{
	GetWorldTimerManager().SetTimer(
		RestTimer,
		this,
		&ADarkKnight::EndRestTimer,
		1.5f,
		false);
}

void ADarkKnight::EndRestTimer()
{
	if (EnemyAIController->GetBlackboardComponent()->GetValueAsBool(TEXT("InAttackRange"))) {
		ChangeCombatState(ECombatState::ECS_Unoccupied);
	}
	else {
		ChangeCombatState(ECombatState::ECS_Chase);
		EnemyAIController->GetBlackboardComponent()->SetValueAsInt(TEXT("ActionIndex"), 1);
	}
}

void ADarkKnight::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsRotate) {
		if (UKismetMathLibrary::EqualEqual_RotatorRotator(GetActorRotation(), LastSaveRotate)) {
			bIsRotate = false;
		}
		else {
			SetActorRotation(UKismetMathLibrary::RInterpTo(GetActorRotation(), LastSaveRotate, DeltaTime, InterpSpeed));
		}
	}
}