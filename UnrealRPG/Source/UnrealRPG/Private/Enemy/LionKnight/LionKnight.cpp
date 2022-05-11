// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/LionKnight/LionKnight.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Magic/ProjectileMagic.h"
#include "Enemy/EnemyAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Player/PlayerCharacter.h"
#include "AlgorithmStaticLibrary.h"

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

void ALionKnight::ChooseNextAttack()
{
	//   LionKnight.cpp				Enemy.cpp				Enemy.cpp
	// [ 후방 공격 확인 ] - - - [ 특수 공격 확인 ] - - - [ 일반 공격 실행 ]

	/* 후방 공격 실행 조건
	* 1. 공격이 존재하는 경우
	* 2. 플레이어가 뒤에 있는 경우 ( abs(110~180) )
	* 3. WaitCount가 0인 경우
	*/
	const bool bValidBackAttack{ BackAttackList.Num() > 0 };
	const bool bBehindPlayer{ abs(GetDegreeForwardToTarget()) >= 110.f };
	
	if (bValidBackAttack && bBehindPlayer)
	{
		EnemySpecialAttackLinkedListNode* SelectMontageNodePtr{ BackAttackList.GetHead() };
		FEnemySpecialAttack& SelectMontageRef{ SelectMontageNodePtr->GetValue() };
	
		const bool bZeroCount{ SelectMontageRef.WaitCount == 0 };
		if (bZeroCount)
		{
			SelectMontageRef.WaitCount = SelectMontageRef.MaximumWaitCount;

			NextOrPlayingAttack = &SelectMontageRef;
	
			BackAttackList.RemoveNode(SelectMontageNodePtr, false);
			BackAttackList.AddTail(SelectMontageNodePtr);

			// 공격을 선택했으니 종료
			return;
		}
		else
		{
			SelectMontageRef.WaitCount--;
		}
	}

	// Special, Advanced 중 하나 실행
	Super::ChooseNextAttack();
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
	InitAttackMontage();
}

void ALionKnight::InitPage_Implementation()
{
	bSecondPageUp = false;
	InitAttackMontage();
}

void ALionKnight::PlayNextPageMontage()
{
	if (NextPageMontage)
	{
		AnimInstance->Montage_Play(NextPageMontage);

		EnemyAIController->GetBlackboardComponent()->SetValueAsBool(TEXT("bEvent"), true);
	}
}

void ALionKnight::InitAttackMontage()
{
	AdvancedAttackList.Empty();
	SpecialAttackList.Empty();

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
		TArray<FEnemyAdvancedAttack> AdvancedAttack{ SkillSetDataRow->AdvancedAttackMontage };
		TArray<FEnemySpecialAttack> SpecialAttack{ SkillSetDataRow->SpecialAttackMontage };
		TArray<FEnemySpecialAttack> BackAttack{ SkillSetDataRow->BackAttackMontage };

		if (bRandomAttackMontage)
		{
			UAlgorithmStaticLibrary::ShuffleArray(AdvancedAttack);
			UAlgorithmStaticLibrary::ShuffleArray(SpecialAttack);
			UAlgorithmStaticLibrary::ShuffleArray(BackAttack);
		}

		// AdvancedAttackList에 정보를 저장한다.
		for (const FEnemyAdvancedAttack& AttackMontage : AdvancedAttack)
		{
			AdvancedAttackList.AddTail(AttackMontage);
		}
	
		// SpecialAttackList에 정보를 저장한다.
		for (FEnemySpecialAttack AttackMontage : SpecialAttack)
		{
			if (AttackMontage.bResetWaitCount)
			{
				AttackMontage.WaitCount = AttackMontage.MaximumWaitCount;
			}
	
			SpecialAttackList.AddTail(AttackMontage);
		}

		// BackAttackList에 정보를 저장한다.
		for (FEnemySpecialAttack AttackMontage : BackAttack)
		{
			if (AttackMontage.bResetWaitCount)
			{
				AttackMontage.WaitCount = AttackMontage.MaximumWaitCount;
			}

			BackAttackList.AddTail(AttackMontage);
		}
		
		DodgeMontage = SkillSetDataRow->DodgeMontage;
		ProjectileMagic = SkillSetDataRow->ProjectileMagic;
		ProjectileDamage = SkillSetDataRow->ProjectileMagicDamage;
	
		ChooseNextAttack();
	}
}
