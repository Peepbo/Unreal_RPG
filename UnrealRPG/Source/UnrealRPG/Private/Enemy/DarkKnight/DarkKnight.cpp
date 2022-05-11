// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/DarkKnight/DarkKnight.h"
#include "Enemy/DarkKnight/KnightAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/PlayerCharacter.h"
#include "Enemy/EnemyAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/WidgetComponent.h"


ADarkKnight::ADarkKnight() :
	bShouldDrawWeapon(true),
	DirectionLerpSpeed(1.f),
	TurnTime(1.2f)
{
	AD = 20.f;
	GetWeapon()->SetVisibility(false);
	GetLockOnWidget()->SetRelativeLocation({ 0.f,0.f,100.f });

	WeaponCaseMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponCase"));
	WeaponCaseMesh->SetupAttachment(GetMesh(), TEXT("SwordSocket"));
}

void ADarkKnight::BeginPlay()
{
	Super::BeginPlay();

	if (AnimInstance) {
		UKnightAnimInstance* KnightAnimInst = Cast<UKnightAnimInstance>(AnimInstance);

		if (KnightAnimInst) {
			KnightAnimInstance = KnightAnimInst;
		}
	}

	ChangeEnemySize(EEnemySize::EES_Medium);

	EnemyAIController->GetBlackboardComponent()->SetValueAsFloat(TEXT("TurnTime"), TurnTime);
	EnemyAIController->GetBlackboardComponent()->SetValueAsBool(TEXT("bFirstPatrol"), true);
	EnemyAIController->GetBlackboardComponent()->SetValueAsBool(TEXT("bShouldFaceOff"), true);

	// BattleMode에 해당하는 세팅으로 변경한다.
	if (bIsBattleMode)
	{
		GetWeapon()->SetVisibility(true);
		WeaponCaseMesh->SetVisibility(false);
		bShouldDrawWeapon = false;
	}
	else
	{
		GetWeapon()->SetVisibility(false);
		WeaponCaseMesh->SetVisibility(true);
		bShouldDrawWeapon = true;
	}
	EnemyAIController->GetBlackboardComponent()->SetValueAsBool(TEXT("IsBattleMode"), bIsBattleMode);
}

void ADarkKnight::SetBackWeaponVisibility(const bool bNextVisibility)
{
	WeaponCaseMesh->SetVisibility(bNextVisibility);
}

void ADarkKnight::SetEquipWeaponVisibility(const bool bNextVisibility)
{
	GetWeapon()->SetVisibility(bNextVisibility);
}

void ADarkKnight::AgroSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//if (!bShouldDrawWeapon)return;
	if (Target != nullptr)return;
	//if (bIsBattleMode)return;
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

void ADarkKnight::EndDraw()
{
	// 강제로 Impact State를 막았던 설정을 되돌려놓는다.
	bAvoidImpactState = false;
	ChangeBattleMode();
}

void ADarkKnight::StartSheath()
{
	GetCharacterMovement()->StopActiveMovement();
	SetMove(false);
	if (SheathMontage) {
		if (AnimInstance) {
			AnimInstance->Montage_Play(SheathMontage);
			UE_LOG(LogTemp, Warning, TEXT("Sheath Montage Play"));
		}
	}
}

void ADarkKnight::EndSheath()
{
	bShouldDrawWeapon = true;
	OverlapCharacter = nullptr;
	ChangeBattleMode();
}

void ADarkKnight::EndDamageImpact()
{
	Super::EndDamageImpact();

	if (bShouldDrawWeapon) {
		if (Target && EnemyAIController) {
			GetWorldTimerManager().ClearTimer(SearchTimer);
			ChangeColliderSetting(true);
		
			bShouldDrawWeapon = false;
			EnemyAIController->GetBlackboardComponent()->SetValueAsObject(TEXT("Target"), Target);
		
			StartDraw();
		}
	}
}

void ADarkKnight::PlayAttackMontage()
{
	if (NextOrPlayingAttack->AttackMontage != nullptr)
	{
		if (GetSprinting() && SprintAttack.AttackMontage)
		{
			AnimInstance->Montage_Play(SprintAttack.AttackMontage);
			SetSprinting(false);
		}
		else
		{
			if (NextOrPlayingAttack->AttackMontage)
			{
				AnimInstance->Montage_Play(NextOrPlayingAttack->AttackMontage);
			}
		}

		CombatResetTime = 0.f;
	}
}

void ADarkKnight::FindCharacter()
{
	Super::FindCharacter();

	if (Target && bShouldDrawWeapon)
	{
		bShouldDrawWeapon = false;

		StartDraw();
	}
}

void ADarkKnight::ResetCombat()
{
	CombatResetTime = 0.f;
	Target = nullptr;
	EnemyAIController->GetBlackboardComponent()->SetValueAsObject(TEXT("Target"), nullptr);

	SetVisibleHealthBar(false);
	StartSheath();

	if (!bPatrolableEnemy)
	{
		StartResetTransformTimer(2.f);
	}
}

void ADarkKnight::DropWeapon()
{
	GetWeapon()->SetSimulatePhysics(true);
	GetWeapon()->SetCollisionObjectType(ECollisionChannel::ECC_PhysicsBody);
}

void ADarkKnight::ChangeBeginBattleMode()
{
	ChangeBattleMode();

	if (bIsBattleMode)
	{
		GetWeapon()->SetVisibility(true);
		WeaponCaseMesh->SetVisibility(false);
		bShouldDrawWeapon = false;
	}
	else
	{
		GetWeapon()->SetVisibility(false);
		WeaponCaseMesh->SetVisibility(true);
		bShouldDrawWeapon = true;
	}
}

void ADarkKnight::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

bool ADarkKnight::CustomTakeDamage(float DamageAmount, AActor* DamageCauser, EAttackType AttackType)
{
	Super::CustomTakeDamage(DamageAmount, DamageCauser, AttackType);

	UE_LOG(LogTemp, Warning, TEXT("DarkKnight Damaged!"));

	if (!bDying) 
	{
		GetWorldTimerManager().ClearTimer(SearchTimer);
		if (Target == nullptr)
		{
			APlayerCharacter* Player = Cast<APlayerCharacter>(DamageCauser);
			if (Player)
			{
				Target = Player;
			}
			else
			{
				return false;
			}
		}

		// 강제로 ImpactState로 전환되는 것을 막는다. (데미지는 들어감)
		// Non-Battle Impact, DrawAnimation 모두 끝나면 false로 바뀐다.
		if (bShouldDrawWeapon)
		{
			bAvoidImpactState = true;
		}
	}


	if (bDying) 
	{
		FDetachmentTransformRules DetachmentTransfromRules(EDetachmentRule::KeepWorld, true);
		GetWeapon()->DetachFromComponent(DetachmentTransfromRules);
		GetWorldTimerManager().SetTimer(DropWeaponTimer, this, &ADarkKnight::DropWeapon, 0.1f, false);
	}
	else
	{
		CombatResetTime = 0.f;
	}

	return true;
}
