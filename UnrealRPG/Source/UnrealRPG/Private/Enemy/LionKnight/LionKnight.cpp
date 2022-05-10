// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/LionKnight/LionKnight.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Magic/ProjectileMagic.h"
#include "Enemy/EnemyAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Player/PlayerCharacter.h"

ALionKnight::ALionKnight():
	ProjectileDamage(50.f),
	bPrepareBattle(false),
	DodgeWaitCount(0),
	MaximumDodgeWaitCount(3),
	DodgeMaximumDistance(500.f),
	NextPageStartHealthPercentage(0.5f),
	bSecondPageUp(false)
{
	GroundedWeapon = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GroundedWeapon"));
	bUseSpecialAttack = true;
	bAutoCombatReset = false;
}

void ALionKnight::BeginPlay()
{
	Super::BeginPlay();

	AD = 30.f;
	//MaximumHP = 2000.f;
	HP = MaximumHP;

	ChangeEnemySize(EEnemySize::EES_Large);

	GetCharacterMovement()->MaxWalkSpeed = MaximumSprintSpeed;

	FTransform GroundedWeaponTransform{ GetMesh()->GetSocketTransform("GroundedWeaponSocket") };
	GroundedWeapon->SetWorldTransform(GroundedWeaponTransform);

	DodgeWaitCount = MaximumDodgeWaitCount;

	// DataTable에서 SkillSet을 받아온다.
	InitSkillSet();
}

void ALionKnight::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ALionKnight::ResetCombat()
{
	// 만약 보스전을 치뤘는데 플레이어가 패배했을 때 호출
	CombatResetTime = 0.f;
	Target = nullptr;
	EnemyAIController->GetBlackboardComponent()->SetValueAsObject(TEXT("Target"), nullptr);
	SetVisibleHealthBar(false);

	GetWorldTimerManager().SetTimer(
		ChangeWeaponSettingTimer,
		this,
		&ALionKnight::ResetBoss,
		4.f);

	StartResetBattleModeTimer(4.f);
	StartResetTransformTimer(4.f);
}

void ALionKnight::EndAttack(bool bChooseNextAttack)
{
	EnemyAIController->ClearFocus(EAIFocusPriority::Gameplay);

	GetCharacterMovement()->bUseControllerDesiredRotation = true;

	// Dodge를 할 수 있을 때 할지 검사한다.
	const bool bPlayDodge{ CheckDodge() };

	// Dodge가 휴식 대신 실행하는 로직이므로 Dodge가 아닐 때만 휴식을 실행해준다.
	if (!bPlayDodge)
	{
		StartRestTimer();
	}

	if (bChooseNextAttack)
	{
		ChooseNextAttack();
	}
}

bool ALionKnight::CustomTakeDamage(float DamageAmount, AActor* DamageCauser, EAttackType AttackType)
{
	Super::CustomTakeDamage(DamageAmount, DamageCauser, AttackType);

	if (!bStun && !bSecondPageUp && GetHpPercentage() <= NextPageStartHealthPercentage)
	{
		bSecondPageUp = true;
		PlayNextPageMontage();
	}
	return true;
}

AProjectileMagic* ALionKnight::UseMagic()
{
	if (ProjectileMagic)
	{
		const FVector MagicStartLoc{ GetActorLocation() + (GetActorForwardVector() * 150.f) };
		AProjectileMagic* SpawnMagic = GetWorld()->SpawnActor<AProjectileMagic>(ProjectileMagic, MagicStartLoc, GetActorRotation());
		SpawnMagic->InitMagic(this, ProjectileDamage);

		return SpawnMagic;
	}
	return nullptr;
}

void ALionKnight::ReadyToBattle()
{
	bPrepareBattle = true;
	EnemyAIController->GetBlackboardComponent()->SetValueAsBool(TEXT("bPrepareBattle"), bPrepareBattle);
}

void ALionKnight::GrapWeapon()
{
	GetWeapon()->SetVisibility(true);
	GroundedWeapon->SetVisibility(false);
}

void ALionKnight::ResetBoss()
{
	// 체력 초기화
	HP = MaximumHP;

	GetWeapon()->SetVisibility(false);
	GroundedWeapon->SetVisibility(true);

	bPrepareBattle = false;
	EnemyAIController->GetBlackboardComponent()->SetValueAsBool(TEXT("bPrepareBattle"), bPrepareBattle);

	InitPage();
}

void ALionKnight::EndDodge()
{
	bDodge = false;
	EnemyAIController->GetBlackboardComponent()->SetValueAsBool(TEXT("bDodge"), false);
}

void ALionKnight::EndEvent()
{
	EnemyAIController->GetBlackboardComponent()->SetValueAsBool(TEXT("bEvent"), false);
}

bool ALionKnight::CheckDodge()
{
	/* Dodge 조건
	*
	* 0. Dodge Montage가 있다.
	* 1. 플레이어가 가까이 있다. (len < 500) (멀리있으면 dodge할 필요가 없다.)
	* 2. 왼쪽, 오른쪽, 뒤 중 한 곳이라도 이동할 수 있다. (이건 나중에)
	* 3. DodgeWaitCount가 0이다.
	*/
	const bool bHaveDodge{ DodgeMontage.Num() > 0 };
	const bool bClosePlayer{ GetDistanceToTarget() < DodgeMaximumDistance };
	const bool bWaitCountZero{ DodgeWaitCount == 0 };
	bool bPlayDodge{ false };

	if (bHaveDodge && bClosePlayer)
	{
		if (bWaitCountZero)
		{
			bPlayDodge = true;
			DodgeWaitCount = MaximumDodgeWaitCount;

			const int32 Rand{ FMath::RandRange(0, DodgeMontage.Num() - 1) };
			bDodge = true;
			AnimInstance->Montage_Play(DodgeMontage[Rand].AttackMontage);
			EnemyAIController->GetBlackboardComponent()->SetValueAsBool(TEXT("bDodge"), bPlayDodge);
			EnemyAIController->GetBlackboardComponent()->SetValueAsBool(TEXT("IsAttack"), false);
			ChangeCombatState(ECombatState::ECS_Unoccupied);
		}
		else
		{
			DodgeWaitCount--;
		}
	}

	return bPlayDodge;
}

void ALionKnight::NextPage_Implementation()
{
	InitSkillSet();
}

void ALionKnight::InitPage_Implementation()
{
	bSecondPageUp = false;
	InitSkillSet();
}

void ALionKnight::PlayNextPageMontage()
{
	if (NextPageMontage)
	{
		AnimInstance->Montage_Play(NextPageMontage);

		EnemyAIController->GetBlackboardComponent()->SetValueAsBool(TEXT("bEvent"), true);
	}
}

void ALionKnight::InitSkillSet()
{
	AdvancedAttackQueue.Empty();
	SpecialAttackQueue.Empty();

	const FString SkillSetTablePath{ "DataTable'/Game/_Game/DataTable/LionKnightSkillDataTable.LionKnightSkillDataTable'" };
	UDataTable* SkillSetTableObject = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, *SkillSetTablePath));
	if (SkillSetTableObject == nullptr)
	{
		return;
	}

	FLionKnightSkillSet* SkillSetDataRow = nullptr;
	FName RowName{ !bSecondPageUp ? FName("Page1") : FName("Page2") };

	SkillSetDataRow = SkillSetTableObject->FindRow<FLionKnightSkillSet>(RowName, TEXT(""));

	if (SkillSetDataRow)
	{
		TArray<FEnemyAdvancedAttack> AdvancedAttack = SkillSetDataRow->AdvancedAttackMontage;
		for (FEnemyAdvancedAttack AttackMontage : AdvancedAttack)
		{
			if (AttackMontage.bResetWaitCount)
			{
				AttackMontage.WaitCount = AttackMontage.MaximumWaitCount;
			}
			
			AdvancedAttackQueue.Enqueue(AttackMontage);
		}

		TArray<FEnemyAdvancedAttack> SpecialAttack = SkillSetDataRow->SpecialAttackMontage;
		for (FEnemyAdvancedAttack AttackMontage : SpecialAttack)
		{
			if (AttackMontage.bResetWaitCount)
			{
				AttackMontage.WaitCount = AttackMontage.MaximumWaitCount;
			}

			SpecialAttackQueue.Enqueue(AttackMontage);
		}

		DodgeMontage = SkillSetDataRow->DodgeMontage;
		ProjectileMagic = SkillSetDataRow->ProjectileMagic;
		ProjectileDamage = SkillSetDataRow->ProjectileMagicDamage;

		ChooseNextAttack();
	}
}
