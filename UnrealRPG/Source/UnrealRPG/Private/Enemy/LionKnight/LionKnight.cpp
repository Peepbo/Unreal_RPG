// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/LionKnight/LionKnight.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Magic/Magic.h"
#include "Enemy/EnemyAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Player/PlayerCharacter.h"
#include "AlgorithmStaticLibrary.h"

ALionKnight::ALionKnight():
	bPrepareBattle(false),
	NextPageStartHealthPercentage(0.5f),
	bSecondPageUp(false),
	bEvent(false)
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
	bAvoidImpactState = true;

	FTransform GroundedWeaponTransform{ GetMesh()->GetSocketTransform("GroundedWeaponSocket") };
	GroundedWeapon->SetWorldTransform(GroundedWeaponTransform);


	if (bRandomAttackMontage)
	{
		UAlgorithmStaticLibrary::ShuffleArray(DodgeMontage);
		for (FEnemySpecialAttack& Attack : DodgeMontage)
		{
			if (Attack.bResetWaitCount)
			{
				Attack.WaitCount = Attack.MaximumWaitCount;
			}
			DodgeMontageList.AddTail(Attack);
		}
	}

	AttackManager->SetAttackSequence({ EEnemyMontageType::EEMT_Back, EEnemyMontageType::EEMT_Magic, EEnemyMontageType::EEMT_Special });
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

void ALionKnight::EndAttack()
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
}

bool ALionKnight::CustomTakeDamage(float DamageAmount, AActor* DamageCauser, EAttackType AttackType)
{
	Super::CustomTakeDamage(DamageAmount, DamageCauser, AttackType);

	if (!bStun && !bSecondPageUp && GetHpPercentage() <= NextPageStartHealthPercentage)
	{
		bSecondPageUp = true;
		PlayNextPageMontage();
	}

	if (bDying)
	{
		CallDieDispatcher();
	}
	return true;
}

void ALionKnight::InitStat()
{
	const FString BossStatTablePath{ "DataTable'/Game/_Game/DataTable/BossStatDataTable.BossStatDataTable'" };
	UDataTable* BossStatTableObject = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, *BossStatTablePath));

	if (BossStatTableObject != nullptr)
	{
		FBossStatDataTable* StatDataRow = nullptr;
		StatDataRow = BossStatTableObject->FindRow<FBossStatDataTable>(EnemyDataTableRowName, TEXT(""));

		if (StatDataRow != nullptr)
		{
			CharacterName = StatDataRow->CharacterName;
			EnemySize = StatDataRow->EnemySize;
			LockOnSocketName = StatDataRow->LockOnSocketName;
			MaximumHP = StatDataRow->MaximumHP;
			RewardGold = StatDataRow->RewardGold;

			MaximumMentality = StatDataRow->MaximumMentality;
		}
	}
}

AMagic* ALionKnight::UseMagic()
{
	if (AttackManager->GetMontageType() != EEnemyMontageType::EEMT_Magic)
	{
		return nullptr;
	}

	if (AttackManager->GetLastMagic() != nullptr)
	{
		const FVector MagicStartLoc{ GetActorLocation() + (GetActorForwardVector() * 150.f) };
		AMagic* SpawnMagic = GetWorld()->SpawnActor<AMagic>(AttackManager->GetLastMagic(), MagicStartLoc, GetActorRotation());
		SpawnMagic->InitMagic(this, AttackManager->GetLastMagicDamage());

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
	EnemyAIController->ClearFocus(EAIFocusPriority::Gameplay);

	bDodge = false;
	EnemyAIController->GetBlackboardComponent()->SetValueAsBool(TEXT("bDodge"), false);

	ChooseNextAttack();
}

void ALionKnight::EndEvent()
{
	ChooseNextAttack();

	bEvent = false;
	EnemyAIController->GetBlackboardComponent()->SetValueAsBool(TEXT("bEvent"), bEvent);
}

bool ALionKnight::CheckDodge()
{
	if (DodgeMontageList.GetHead() == nullptr)
	{
		return false;
	}
	if (AttackManager->GetMontageType() == EEnemyMontageType::EEMT_Back)
	{
		return false;
	}
	if (bEvent)
	{
		return false;
	}
	//EnemyAIController->GetBlackboardComponent()->SetValueAsBool(TEXT("bEvent"), true);
	/* Dodge 조건
	*
	* 1. 플레이어가 가까이 있다. (len < 500) (멀리있으면 dodge할 필요가 없다.)
	* 2. 왼쪽, 오른쪽, 뒤 중 한 곳이라도 이동할 수 있다. (이건 나중에)
	* 3. DodgeWaitCount가 0이다.
	*/
	EnemySpecialAttackLinkedListNode* DodgeListNodePtr{ DodgeMontageList.GetHead() };
	FEnemySpecialAttack& DodgeDataRef{ DodgeMontageList.GetHead()->GetValue() };

	const bool bClosePlayer{ GetDistanceToTarget() < DodgeDataRef.AttackAbleDistance };
	const bool bWaitCountZero{ DodgeDataRef.WaitCount == 0 };
	bool bPlayDodge{ false };

	if (bClosePlayer)
	{
		if (bWaitCountZero)
		{
			bPlayDodge = true;
			bDodge = true;

			DodgeDataRef.WaitCount = DodgeDataRef.MaximumWaitCount;
			DodgeMontageList.RemoveNode(DodgeListNodePtr, false);
			DodgeMontageList.AddTail(DodgeListNodePtr);

			AnimInstance->Montage_Play(DodgeDataRef.AttackMontage);

			EnemyAIController->GetBlackboardComponent()->SetValueAsBool(TEXT("bDodge"), bPlayDodge);
			EnemyAIController->GetBlackboardComponent()->SetValueAsBool(TEXT("IsAttack"), false);
			ChangeCombatState(ECombatState::ECS_Unoccupied);
		}
		else
		{
			DodgeDataRef.WaitCount--;
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
		EnemyAIController->GetBlackboardComponent()->SetValueAsBool(TEXT("bDodge"), false);
		EnemyAIController->GetBlackboardComponent()->SetValueAsBool(TEXT("bStun"), false);

		AnimInstance->Montage_Play(NextPageMontage);

		bEvent = true;
		EnemyAIController->GetBlackboardComponent()->SetValueAsBool(TEXT("bEvent"), bEvent);
	}
}

void ALionKnight::BreakGround()
{
	// 플레이어가 전방 60도(-60~+60), 300 안에 들었을 때 데미지를 입힌다.
	const bool bCheckDegree{ abs(GetDegreeForwardToTarget()) <= 60.f };
	const bool bCheckDistance{ GetDistanceToTarget() <= 750.f };

	const bool bApplyDamage{ bCheckDegree && bCheckDistance };
	if (bApplyDamage && !Target->GetImpacting())
	{
		Target->CustomApplyDamage(Target, AttackManager->GetAttackDamage(), this, AttackManager->GetAttackType());
	}
}

void ALionKnight::SplashDamage(bool bDefaultDamage, float SelectDamage)
{
	if (Target == nullptr)
	{
		return;
	}

	float Damage = SelectDamage;

	if (bDefaultDamage)
	{
		Damage = !bSecondPageUp ? 10.f : 15.f;
	}
	
	FVector SocketLocation{ GetMesh()->GetSocketLocation("WeaponFront") };
	SocketLocation.Z = 0.f;

	FVector TargetLocation{ Target->GetActorLocation() };
	TargetLocation.Z = 0.f;

	const float Distance{ (TargetLocation - SocketLocation).Size() };

	if (Distance <= 230.f && !Target->GetImpacting() && !Target->GetCharacterMovement()->IsFalling())
	{
		Target->CustomApplyDamage(Target, Damage, this, EAttackType::EAT_Light);
	}
}

void ALionKnight::InitAttackMontage()
{
	DodgeMontageList.Empty();

	const FString SkillSetTablePath{ "DataTable'/Game/_Game/DataTable/BossSkillSetDataTable.BossSkillSetDataTable'" };
	UDataTable* SkillSetTableObject = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, *SkillSetTablePath));
	if (SkillSetTableObject == nullptr)
	{
		return;
	}
	
	FBossSkillSet* SkillSetDataRow = nullptr;

	FString RowString{ EnemyDataTableRowName.ToString() };
	const int32 PageIndex{ !bSecondPageUp ? 1 : 2 };
	RowString.Append(FString::FromInt(PageIndex));
	
	SkillSetDataRow = SkillSetTableObject->FindRow<FBossSkillSet>(FName(*RowString), TEXT(""));
	
	if (SkillSetDataRow != nullptr)
	{
		TArray<FEnemyNormalAttack> NormalAttack{ SkillSetDataRow->NormalAttackMontage };
		TArray<FEnemySpecialAttack> SpecialAttack{ SkillSetDataRow->SpecialAttackMontage };
		TArray<FEnemySpecialAttack> BackAttack{ SkillSetDataRow->BackAttackMontage };
		TArray<FEnemyMagicAttack> MagicAttack{ SkillSetDataRow->MagicAttackMontage };

		AttackManager->SetShuffle(bRandomAttackMontage);
		AttackManager->InitMontageData(NormalAttack, SpecialAttack, BackAttack, MagicAttack);
		
		DodgeMontage = SkillSetDataRow->DodgeMontage;
	}
}

void ALionKnight::RecoverStun()
{
	Super::RecoverStun();

	if (!bSecondPageUp && GetHpPercentage() <= NextPageStartHealthPercentage)
	{
		bSecondPageUp = true;
		PlayNextPageMontage();
	}
}
