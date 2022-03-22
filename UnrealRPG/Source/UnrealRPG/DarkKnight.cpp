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
	bTurnInPlace(false),
	InterpSpeed(5.f),
	bAttackable(true),
	WalkDirection(0.f),
	DirectionLerpSpeed(1.f),
	TurnTime(1.2f),
	bMove(false)
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

	EnemyAIController->GetBlackboardComponent()->SetValueAsFloat(TEXT("TurnTime"), TurnTime);
	EnemyAIController->GetBlackboardComponent()->SetValueAsBool(TEXT("bFirstPatrol"), true);
}

void ADarkKnight::AgroSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!bShouldDrawWeapon)return;
	if (bIsBattleMode)return;
	if (OtherActor == nullptr)return;

	auto Character = Cast<APlayerCharacter>(OtherActor);
	if (Character) {
		OverlapCharacter = Character;

		// 타이머를 돌리는 이유? beginOverlap은 한번만 작동하므로 적이 overlap상태일 때 조건에 맞지 않으면 영영 캐릭터를 찾지 못하게 된다.
		// 고로 타이머를 돌려 특정 딜레이 마다 조건을 검사한다.
		GetWorldTimerManager().SetTimer(
			SearchTimer,
			this,
			&ADarkKnight::FindCharacter,
			0.05f,
			true);
	}
}

void ADarkKnight::AgroSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	GetWorldTimerManager().ClearTimer(SearchTimer);
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
		GetCharacterMovement()->bUseControllerDesiredRotation = false;

		if (GetSprinting() && SprintAttackMontage) {
			AnimInstance->Montage_Play(SprintAttackMontage);
			SetSprinting(false);

			UE_LOG(LogTemp, Warning, TEXT("Play Sprint Attack Montage"));
		}
		else {
			if (AttackMontage.IsValidIndex(AttackIndex) && AttackMontage[AttackIndex]) {
				AnimInstance->Montage_Play(AttackMontage[AttackIndex]);
				LastAttackIndex = AttackIndex;

				UE_LOG(LogTemp, Warning, TEXT("Play Attack Montage"));
			}

			AttackIndex++;
			if (AttackMontage.Num() == AttackIndex) {
				AttackIndex = 0;
			}
		}
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
	bTurnInPlace = true;
	EnemyAIController->GetBlackboardComponent()->SetValueAsBool(TEXT("bRotate"), bTurnInPlace);
}

void ADarkKnight::StopRotate()
{
	bTurnInPlace = false;
	EnemyAIController->GetBlackboardComponent()->SetValueAsBool(TEXT("bRotate"), bTurnInPlace);
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
		bVisibleTraceSphere? EDrawDebugTrace::ForDuration: EDrawDebugTrace::None,
		HitResult,
		true
	);

	if (bHit) {
		if (HitResult.Actor != nullptr) {
			auto Player = Cast<APlayerCharacter>(HitResult.Actor);
	
			if (Player) {
				// attack point를 플레이어한테 전달
				Player->SetHitPoint(HitResult.Location);

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

void ADarkKnight::FaceOff(float NextWalkDirection)
{
	GetCharacterMovement()->bUseControllerDesiredRotation = false;

	WalkDirection = NextWalkDirection;
}

void ADarkKnight::EndFaceOff()
{
	GetCharacterMovement()->bUseControllerDesiredRotation = true;

	WalkDirection = 0.f;
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
	EnemyAIController->GetBlackboardComponent()->SetValueAsBool(TEXT("IsAttack"), false);
	ChangeCombatState(ECombatState::ECS_Unoccupied);

	GetCharacterMovement()->bUseControllerDesiredRotation = true;
}

void ADarkKnight::ChangeSprinting(bool IsSprinting)
{
	SetSprinting(IsSprinting);
	if (IsSprinting) {
		GetCharacterMovement()->MaxWalkSpeed = BattleRunSpeed;
	}
	else {
		GetCharacterMovement()->MaxWalkSpeed = MaximumWalkSpeed;
	}
}

void ADarkKnight::FindCharacter()
{
	// 캐릭터가 전방 160도 안에 있는지 확인한다. (dot product)
	FVector ActorForward{ GetActorForwardVector() };
	ActorForward.Z = 0.f;

	FVector ActorToCharacter{ OverlapCharacter->GetActorLocation() - GetActorLocation() };
	ActorToCharacter.Z = 0.f;
	ActorToCharacter = UKismetMathLibrary::Normal(ActorToCharacter);

	float DotRadian{ UKismetMathLibrary::Dot_VectorVector(ActorForward, ActorToCharacter) };
	float DotDegree{ UKismetMathLibrary::DegAcos(DotRadian) };
	// 전방 160도면 -80~+80 사이 (dot product는 어차피 -가 나오질 않으므로 80이하인지 검사)
	const bool bFind{ DotDegree <= 80.f };

	if (bFind) {
		if (EnemyAIController) {
			GetWorldTimerManager().ClearTimer(SearchTimer);
			ChangeColliderSetting(true);
			
			StopRotate();

			bShouldDrawWeapon = false;

			Target = OverlapCharacter;
			EnemyAIController->GetBlackboardComponent()->SetValueAsObject(TEXT("Target"), OverlapCharacter);

			StartDraw();
		}
	}
}

float ADarkKnight::GetDegreeForwardToTarget()
{
	if (Target == nullptr)return 0.f;

	const FVector ActorForward{ GetActorForwardVector() };
	const FVector ActorToTarget{ UKismetMathLibrary::Normal(Target->GetActorLocation() - GetActorLocation()) };
	const FVector2D ActorForward2D{ ActorForward };
	const FVector2D ActorToTarget2D{ ActorToTarget };

	const float DotProductRadian{ UKismetMathLibrary::DotProduct2D(ActorForward2D,ActorToTarget2D) };
	float ResultDegree{ UKismetMathLibrary::DegAcos(DotProductRadian) };

	const float CrossProduct{ UKismetMathLibrary::CrossProduct2D(ActorForward2D,ActorToTarget2D) };
	if (CrossProduct < 0.f) {
		ResultDegree = -(ResultDegree);
	}

	return ResultDegree;
}

void ADarkKnight::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (GetAttacking() && bTurnInPlace) {
		const FRotator LookRot{ UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Target->GetActorLocation()) };

		if (UKismetMathLibrary::EqualEqual_RotatorRotator(GetActorRotation(), { 0.f,LookRot.Yaw,0.f }, 0.5f)) {
			bTurnInPlace = false;
			GetCharacterMovement()->bUseControllerDesiredRotation = true;
			UE_LOG(LogTemp, Warning, TEXT("LookAtYaw almost same"), LookRot.Yaw);
		}
		else {
			UE_LOG(LogTemp, Warning, TEXT("LookAtYaw : %f"), LookRot.Yaw);

			SetActorRotation(UKismetMathLibrary::RInterpTo(GetActorRotation(), { 0.f,LookRot.Yaw,0.f }, DeltaTime, InterpSpeed));
		}
	}
}

float ADarkKnight::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (!bDying) {
		UE_LOG(LogTemp, Warning, TEXT("DarkKnight Damaged!"));

		if (bShouldDrawWeapon) {
			APlayerCharacter* Player = Cast<APlayerCharacter>(DamageCauser);
			if (Player && EnemyAIController) {
				GetWorldTimerManager().ClearTimer(SearchTimer);
				ChangeColliderSetting(true);

				bShouldDrawWeapon = false;

				Target = Player;
				EnemyAIController->GetBlackboardComponent()->SetValueAsObject(TEXT("Target"), Player);

				StartDraw();
			}
		}
	}

	return DamageAmount;
}
