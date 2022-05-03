// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Enemy.h"
#include "Kismet/KismetMathLibrary.h"
#include "DrawDebugHelpers.h"
#include "Enemy/EnemyAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Player/PlayerCharacter.h"
#include "Components/WidgetComponent.h"
#include "Components/CapsuleComponent.h"

#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

// Sets default values
AEnemy::AEnemy() :
	bInAttackRange(false),
	BattleWalkSpeed(130.f),
	BattleRunSpeed(400.f),
	bIsSprint(false),
	EnemySize(EEnemySize::EES_MAX),
	bLockOnEnemy(false),
	bRestTime(false),
	bAvoidImpactState(false),
	bTurn(false),
	bAttackable(true),
	WalkDirection(0.f),
	bMove(false),
	InplaceRotateSpeed(5.f),
	AttackRotateSpeed(5.f),
	AttackIndex(0),
	OverrideHP(0.f),
	bActiveBoneOffset(false),
	MaximumCombatResetTime(20.f),
	MaximumCombatDistance(2500.f),
	CombatResetTime(0.f),
	bPatrolableEnemy(false)
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AgroSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AgroSphere"));
	AgroSphere->SetupAttachment(GetRootComponent());
	AgroSphere->ComponentTags.Add(TEXT("Agro"));

	CombatRangeSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CombatRangeSphere"));
	CombatRangeSphere->SetupAttachment(GetRootComponent());
	CombatRangeSphere->ComponentTags.Add(TEXT("CombatRange"));

	MaximumWalkSpeed = 170.f;
	GetCharacterMovement()->MaxWalkSpeed = MaximumWalkSpeed;
	GetCharacterMovement()->bAllowPhysicsRotationDuringAnimRootMotion = true;

	// Widget
	LockOnWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("LockOnWidget"));
	LockOnWidget->SetupAttachment(GetMesh(), TEXT("Hips"));
	LockOnWidget->SetWidgetSpace(EWidgetSpace::Screen);
	LockOnWidget->SetDrawSize({ 18.f,18.f });
	LockOnWidget->SetVisibility(false);

	HealthBar = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBarWidget"));
	HealthBar->SetupAttachment(GetMesh());
	HealthBar->SetWidgetSpace(EWidgetSpace::Screen);
	HealthBar->SetDrawSize({ 150.f,10.f });
	HealthBar->SetVisibility(false);

	// Weapon
	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon"));
	WeaponMesh->SetupAttachment(GetMesh(), TEXT("RightHandSocket"));

}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	AgroSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::AgroSphereOverlap);
	AgroSphere->OnComponentEndOverlap.AddDynamic(this, &AEnemy::AgroSphereEndOverlap);

	CombatRangeSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::CombatRangeOverlap);
	CombatRangeSphere->OnComponentEndOverlap.AddDynamic(this, &AEnemy::CombatRangeEndOverlap);
	// 플레이어를 찾게 되면 activate 해준다.
	CombatRangeSphere->Deactivate();

	EnemyAIController = Cast<AEnemyAIController>(GetController());
	if (EnemyAIController && BehaviorTree)
	{
		EnemyAIController->GetBlackboardComponent()->SetValueAsObject(TEXT("Target"), nullptr);
		EnemyAIController->GetBlackboardComponent()->SetValueAsBool(TEXT("bDying"), false);
		EnemyAIController->GetBlackboardComponent()->SetValueAsBool(TEXT("IsAttack"), false);

		EnemyAIController->RunBehaviorTree(BehaviorTree);
	}

	AnimInstance = GetMesh()->GetAnimInstance();

	FirstTransform = GetActorTransform();
}

void AEnemy::AgroSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (bIsBattleMode)return;
	if (OtherActor == nullptr)return;

	auto Character = Cast<APlayerCharacter>(OtherActor);
	if (Character) {
		if (EnemyAIController) {
			Target = Character;
			EnemyAIController->GetBlackboardComponent()->SetValueAsObject(TEXT("Target"), Character);
		}
	}
}

void AEnemy::AgroSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
}

void AEnemy::CombatRangeOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == nullptr)return;

	auto Character = Cast<APlayerCharacter>(OtherActor);

	if (Character) {
		bInAttackRange = true;

		if (EnemyAIController) {
			EnemyAIController->GetBlackboardComponent()->SetValueAsBool(TEXT("InAttackRange"), true);
		}
	}
}

void AEnemy::CombatRangeEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor == nullptr)return;

	auto Character = Cast<APlayerCharacter>(OtherActor);

	if (Character) 
	{
		bInAttackRange = false;

		if (EnemyAIController)
		{
			EnemyAIController->GetBlackboardComponent()->SetValueAsBool(TEXT("InAttackRange"), false);
		}
	}
}

void AEnemy::ChangeBattleMode()
{
	bIsBattleMode = !bIsBattleMode;

	GetCharacterMovement()->MaxWalkSpeed = (bIsBattleMode ? BattleWalkSpeed : MaximumWalkSpeed);

	if (EnemyAIController)
	{
		EnemyAIController->GetBlackboardComponent()->SetValueAsBool(TEXT("IsBattleMode"), bIsBattleMode);
	}
}

void AEnemy::PlayAttackMontage()
{
	if (AdvancedAttackMontage.IsValidIndex(0) && AdvancedAttackMontage[0].AttackMontage)
	{
		AnimInstance->Montage_Play(AdvancedAttackMontage[0].AttackMontage);
	}
}

void AEnemy::ChangeEnemySize(EEnemySize Size)
{
	EnemySize = Size;

	switch (EnemySize)
	{
	case EEnemySize::EES_Small:
		LockOnMinimumPitchValue = -30.f;
		break;
	case EEnemySize::EES_Medium:
		LockOnMinimumPitchValue = -20.f;
		break;
	case EEnemySize::EES_Large:
		LockOnMinimumPitchValue = -10.f;
		break;
	}
}

void AEnemy::ChangeColliderSetting(bool bBattle)
{
	if (bBattle) {
		AgroSphere->Deactivate();
		CombatRangeSphere->Activate();
	}
	else {
		AgroSphere->Activate();
		CombatRangeSphere->Deactivate();
	}
}

void AEnemy::Die()
{
	// RagDoll로 바꿔준다.
	GetMesh()->SetCollisionProfileName(FName("Ragdoll"));
	GetMesh()->SetSimulatePhysics(true);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AEnemy::SaveTargetRotator()
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

void AEnemy::StartRotate()
{
	bTurn = true;
	EnemyAIController->GetBlackboardComponent()->SetValueAsBool(TEXT("bRotate"), bTurn);
}

void AEnemy::StopRotate()
{
	bTurn = false;
	EnemyAIController->GetBlackboardComponent()->SetValueAsBool(TEXT("bRotate"), bTurn);
}

void AEnemy::TracingAttackSphere(float Damage)
{
	if (!bAttackable)return;

	/* 아이템의 TopSocket과 BottomSocket의 위치를 받아온다. */
	const FVector TopSocketLoc{ WeaponMesh->GetSocketLocation("TopSocket") };
	const FVector BottomSocketLoc{ WeaponMesh->GetSocketLocation("BottomSocket") };

	FHitResult HitResult;
	// SphereTraceSingle로 원을 그리고 원에 겹치는 오브젝트를 HitResult에 담는다.
	const bool bHit = UKismetSystemLibrary::SphereTraceSingleForObjects(
		this,
		BottomSocketLoc,
		TopSocketLoc,
		50.f,
		// Pawn
		{ EObjectTypeQuery::ObjectTypeQuery3 },
		false,
		{ this },
		bVisibleTraceSphere ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None,
		HitResult,
		true
	);

	if (bHit) {
		if (HitResult.Actor != nullptr) {
			APlayerCharacter* Player = Cast<APlayerCharacter>(HitResult.Actor);

			if (Player) {
				ECombatState PlayerCombatState{ Player->GetCombatState() };

				// attack point를 플레이어한테 전달
				Player->SetHitPoint(HitResult.ImpactPoint);

				// 공격을 상대가 맞았거나, 막았을 때 bAttack이 true가 됨
				bool bAttack = Player->CustomApplyDamage(
					Player,
					Damage,
					this,
					AdvancedAttackMontage[AttackIndex].AttackType);

				if (bAttack)
				{
					// 피해를 입었거나 가드가 부셔졌을 때 파티클, 사운드를 생성한다.
					if (Player->GetImpacting() || Player->GuardBreaking())
					{
						// 피해 파티클이 존재할 때 타격 위치에 파티클을 생성한다.
						if (Player->GetBloodParticle()) {
							UGameplayStatics::SpawnEmitterAtLocation(
								GetWorld(),
								Player->GetBloodParticle(),
								HitResult.ImpactPoint);
						}
						//// 피해 사운드가 존재할 때 타격 위치에 사운드를 생성한다.
						//if (Player->GetBloodSound())
						//{
						//	UGameplayStatics::PlaySoundAtLocation(
						//		this,
						//		Player->GetBloodSound(),
						//		HitResult.ImpactPoint);
						//}
					}

					bAttackable = false;
				}
			}
		}
	}
}

void AEnemy::StartAttackCheckTime(float DamagePersantage)
{
	if (AdvancedAttackMontage.IsValidIndex(AttackIndex) && AdvancedAttackMontage[AttackIndex].AttackMontage)
	{
		FTimerDelegate AttackDelegate;
		AttackDelegate.BindUFunction(
			this,
			FName("TracingAttackSphere"),
			AdvancedAttackMontage[AttackIndex].AttackDamage * DamagePersantage);

		GetWorldTimerManager().SetTimer(
			AttackCheckTimer,
			AttackDelegate,
			0.005f,
			true);
	}
}

void AEnemy::EndAttackCheckTime()
{
	GetWorldTimerManager().ClearTimer(AttackCheckTimer);

	bAttackable = true;
}

void AEnemy::FaceOff(float NextWalkDirection)
{
	CombatState = ECombatState::ECS_FaceOff;

	GetCharacterMovement()->bUseControllerDesiredRotation = false;

	WalkDirection = NextWalkDirection;
}

void AEnemy::EndFaceOff()
{
	CombatState = ECombatState::ECS_Unoccupied;

	GetCharacterMovement()->bUseControllerDesiredRotation = true;

	WalkDirection = 0.f;
}

void AEnemy::Chase()
{
	CombatState = ECombatState::ECS_Chase;
}

void AEnemy::StartRestTimer()
{
	CombatState = ECombatState::ECS_Unoccupied;

	bRestTime = true;
	EnemyAIController->GetBlackboardComponent()->SetValueAsBool(TEXT("bRest"), bRestTime);

	GetWorldTimerManager().SetTimer(
		RestTimer,
		this,
		&AEnemy::EndRestTimer,
		1.1f,
		false);
}

void AEnemy::EndRestTimer()
{
	bRestTime = false;
	EnemyAIController->GetBlackboardComponent()->SetValueAsBool(TEXT("bRest"), bRestTime);

	EnemyAIController->GetBlackboardComponent()->SetValueAsBool(TEXT("IsAttack"), false);
	ChangeCombatState(ECombatState::ECS_Unoccupied);

	//GetCharacterMovement()->bUseControllerDesiredRotation = true;
}

void AEnemy::ChangeSprinting(bool IsSprinting)
{
	SetSprinting(IsSprinting);
	if (IsSprinting) {
		//GetCharacterMovement()->MaxWalkSpeed = BattleRunSpeed;
		GetCharacterMovement()->MaxWalkSpeed = MaximumSprintSpeed;
	}
	else {
		GetCharacterMovement()->MaxWalkSpeed = MaximumWalkSpeed;
	}
}

void AEnemy::FindCharacter()
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

			Target = OverlapCharacter;
			EnemyAIController->GetBlackboardComponent()->SetValueAsObject(TEXT("Target"), OverlapCharacter);
		}
	}
}

float AEnemy::GetDegreeForwardToTarget()
{
	if (Target == nullptr)return 0.f;

	const FVector ActorForward{ GetActorForwardVector() };
	FVector ActorToTarget{ Target->GetActorLocation() - GetActorLocation() };
	ActorToTarget.Z = 0.f;

	const FVector2D ActorForward2D{ ActorForward };
	const FVector2D ActorToTarget2D{ UKismetMathLibrary::Normal(ActorToTarget) };

	const float DotProductRadian{ UKismetMathLibrary::DotProduct2D(ActorForward2D,ActorToTarget2D) };
	float ResultDegree{ UKismetMathLibrary::DegAcos(DotProductRadian) };

	const float CrossProduct{ UKismetMathLibrary::CrossProduct2D(ActorForward2D,ActorToTarget2D) };
	if (CrossProduct < 0.f) {
		ResultDegree = -(ResultDegree);
	}

	return ResultDegree;
}

float AEnemy::GetDistanceToTarget()
{
	if (Target == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("[AEnemy::GetDistanceToTarget()] Target is Nullptr"));
		return 0.f;
	}

	const FVector ToVector{ Target->GetActorLocation() - GetActorLocation() };
	return ToVector.Size();
}

void AEnemy::StartAttack()
{
	ChangeCombatState(ECombatState::ECS_Attack);

	GetCharacterMovement()->bUseControllerDesiredRotation = false;
}

void AEnemy::EndAttack()
{
	EnemyAIController->ClearFocus(EAIFocusPriority::Gameplay);

	GetCharacterMovement()->bUseControllerDesiredRotation = true;
	StartRestTimer();
}

void AEnemy::GetWeaponMesh(USkeletalMeshComponent* ItemMesh)
{
	if (ItemMesh) {
		WeaponMesh = ItemMesh;
	}
}

void AEnemy::PlayMontage(UAnimMontage* Montage)
{
	if (AnimInstance)
	{
		AnimInstance->Montage_Play(Montage);
	}
}

const float AEnemy::GetAttackableDistance() const
{
	if (AdvancedAttackMontage.IsValidIndex(AttackIndex) && AdvancedAttackMontage[AttackIndex].AttackMontage)
	{
		return AdvancedAttackMontage[AttackIndex].AttackAbleRange;
	}

	UE_LOG(LogTemp, Warning, TEXT("AEnemy::GetAttackableDistance : 'Valid'하지않은 'Index'입니다."));
	return 0.f;
}

void AEnemy::SetVisibleHealthBar(bool bVisible)
{
	HealthBar->SetVisibility(bVisible);
}

void AEnemy::StartResetTransformTimer(float Delay)
{
	if (!bPatrolableEnemy)
	{
		// x초 뒤 원래 있던 위치 및 방향을 캐릭터에 적용시킨다.
		GetWorldTimerManager().SetTimer(
			EndCombatTimer,
			this,
			&AEnemy::ResetTransform,
			Delay);
	}
}

void AEnemy::CombatTurn(float DeltaTime)
{
	// 이 함수는 Target이 존재할 때만 호출하는 함수입니다.
	if (bTurn)
	{
		const FRotator LookRot{ UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Target->GetActorLocation()) };

		const float Degree{ GetDegreeForwardToTarget() };
		bTurnLeft = (Degree < 0.f);

		// 공격 회전 속도와 제자리 회전 속도를 다르게한다.
		const float SelectInterpSpeed{ GetAttacking() ? AttackRotateSpeed : InplaceRotateSpeed };
		const float PlusYaw{ (bTurnLeft ? -SelectInterpSpeed : SelectInterpSpeed) * DeltaTime };

		if (UKismetMathLibrary::NearlyEqual_FloatFloat(GetActorRotation().Yaw + PlusYaw, LookRot.Yaw) == false)
		{
			SetActorRotation(UKismetMathLibrary::ComposeRotators(GetActorRotation(), FRotator(0.f, PlusYaw, 0.f)));
		}
	}
}

void AEnemy::CheckCombatReset(float DeltaTime)
{
	// 이 함수는 Target이 존재할 때만 호출하는 함수입니다.
	if (CombatState == ECombatState::ECS_Chase)
	{
		CombatResetTime += DeltaTime;

		if (CombatResetTime > MaximumCombatResetTime || GetDistanceToTarget() > MaximumCombatDistance)
		{
			ResetCombat();
		}
	}
}

void AEnemy::ResetCombat()
{
	CombatResetTime = 0.f;
	Target = nullptr;
	EnemyAIController->GetBlackboardComponent()->SetValueAsObject(TEXT("Target"), nullptr);

	SetVisibleHealthBar(false);
	ChangeBattleMode();
	UE_LOG(LogTemp, Warning, TEXT("AEnemy::ResetCombat()"));


	// Patrol이 적용된 Enemy가 아니면 아직까진 원래 위치로 돌아가는 알고리즘이 없다.
	if (!bPatrolableEnemy)
	{
		StartResetTransformTimer(2.f);
	}
}

void AEnemy::ResetTransform()
{
	SetActorTransform(FirstTransform);
}

void AEnemy::TargetDeath()
{
	//GetWorldTimerManager().SetTimer(
	//	EndCombatTimer,
	//	this,
	//	&AEnemy::ResetCombat,
	//	2.f);
	if (Target)
	{
		Target->ResetLockOn();
	}
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Target)
	{
		if (Target->GetDying())
		{
			GetWorldTimerManager().SetTimer(
				EndCombatTimer,
				this,
				&AEnemy::ResetCombat,
				2.f);

			Target = nullptr;
		}
		else
		{
			CombatTurn(DeltaTime);
			CheckCombatReset(DeltaTime);
		}
	}
}

bool AEnemy::CustomTakeDamage(float DamageAmount, AActor* DamageCauser, EAttackType AttackType)
{
	UE_LOG(LogTemp, Warning, TEXT("Enemy TakeDamage call"));
	if (bDying)
	{
		return false;
	}

	if (HealthBar && !HealthBar->IsWidgetVisible())
	{
		SetVisibleHealthBar(true);
	}

	// Attack이 아니거나 Attack Rest Time일 경우 Impact로 바뀐다.
	if (!bAvoidImpactState && (CombatState != ECombatState::ECS_Attack || bRestTime)) 
	{
		CombatState = ECombatState::ECS_Impact;
	}

	UE_LOG(LogTemp, Warning, TEXT("Enemy Super(MeleeCharacter) TakeDamage call"));
	Super::CustomTakeDamage(DamageAmount, DamageCauser, AttackType);

	UE_LOG(LogTemp, Warning, TEXT("Enemy Change DamageState"));
	DamageState = EDamageState::EDS_invincibility;
	if (bDying) 
	{
		HealthBar->SetVisibility(false);

		EnemyAIController->GetBlackboardComponent()->SetValueAsBool(TEXT("bDying"), true);

		if (AnimInstance && DeathMontage)
		{
			AnimInstance->Montage_Play(DeathMontage);
		}
	}

	return true;
}