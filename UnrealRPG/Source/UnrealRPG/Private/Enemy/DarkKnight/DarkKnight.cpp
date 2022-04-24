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

	//AD = 20.f;
	UE_LOG(LogTemp, Warning, TEXT("%f"), AD);
	if (AnimInstance) {
		UKnightAnimInstance* KnightAnimInst = Cast<UKnightAnimInstance>(AnimInstance);

		if (KnightAnimInst) {
			KnightAnimInstance = KnightAnimInst;
		}
	}

	AttackIndex = FMath::RandRange(0, AdvancedAttackMontage.Num() - 1);

	ChangeEnemySize(EEnemySize::EES_Medium);

	EnemyAIController->GetBlackboardComponent()->SetValueAsFloat(TEXT("TurnTime"), TurnTime);
	EnemyAIController->GetBlackboardComponent()->SetValueAsBool(TEXT("bFirstPatrol"), true);
	EnemyAIController->GetBlackboardComponent()->SetValueAsBool(TEXT("bShouldFaceOff"), true);
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

void ADarkKnight::EndDraw()
{
	// 강제로 Impact State를 막았던 설정을 되돌려놓는다.
	bAvoidImpactState = false;
}

void ADarkKnight::StartSheath()
{
	GetCharacterMovement()->StopActiveMovement();
	SetMove(false);
	if (DrawMontage) {
		if (AnimInstance) {
			AnimInstance->Montage_Play(SheathMontage);
			UE_LOG(LogTemp, Warning, TEXT("Montage Play"));
		}
	}
}

void ADarkKnight::EndSheath()
{
	bShouldDrawWeapon = true;
	OverlapCharacter = nullptr;
}

void ADarkKnight::EndDamageImpact()
{
	Super::EndDamageImpact();

	if (bShouldDrawWeapon) {
		UE_LOG(LogTemp, Warning, TEXT("Draw해야함"));
		if (Target && EnemyAIController) {
			UE_LOG(LogTemp, Warning, TEXT("Target을 지정중"));
			GetWorldTimerManager().ClearTimer(SearchTimer);
			ChangeColliderSetting(true);
		
			bShouldDrawWeapon = false;
			EnemyAIController->GetBlackboardComponent()->SetValueAsObject(TEXT("Target"), Target);
		
			StartDraw();
		}
		else {
			UE_LOG(LogTemp, Warning, TEXT("Target을 지정 실패"));
			if (Target) {
				UE_LOG(LogTemp, Warning, TEXT("%s"), *Target->GetName());
			}
		}
	}
}

void ADarkKnight::PlayAttackMontage()
{
	if (AnimInstance)
	{
		if (GetSprinting() && SprintAttack.AttackMontage)
		{
			AnimInstance->Montage_Play(SprintAttack.AttackMontage);
			SetSprinting(false);
			UE_LOG(LogTemp, Warning, TEXT("Play Sprint Attack Montage"));
		}
		else
		{
			AttackIndex++;
			if (AdvancedAttackMontage.Num() == AttackIndex)
			{
				AttackIndex = 0;
			}

			if (AdvancedAttackMontage.IsValidIndex(AttackIndex) && AdvancedAttackMontage[AttackIndex].AttackMontage)
			{
				AnimInstance->Montage_Play(AdvancedAttackMontage[AttackIndex].AttackMontage);

				UE_LOG(LogTemp, Warning, TEXT("Play Attack Montage"));
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

void ADarkKnight::CheckCombatReset(float DeltaTime)
{
	// 이 함수는 Target이 존재할 때만 호출하는 함수입니다.
	if (CombatState == ECombatState::ECS_Chase)
	{
		CombatResetTime += DeltaTime;

		if (CombatResetTime > MaximumCombatResetTime || GetDistanceToTarget() > MaximumCombatDistance)
		{
			CombatResetTime = 0.f;
			Target = nullptr;
			EnemyAIController->GetBlackboardComponent()->SetValueAsObject(TEXT("Target"), nullptr);

			SetVisibleHealthBar(false);
			StartSheath();
			ChangeBattleMode();
		}
	}
}

void ADarkKnight::DropWeapon()
{
	GetWeapon()->SetSimulatePhysics(true);
	GetWeapon()->SetCollisionObjectType(ECollisionChannel::ECC_PhysicsBody);
}

void ADarkKnight::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

bool ADarkKnight::CustomTakeDamage(float DamageAmount, AActor* DamageCauser, EAttackType AttackType)
{
	Super::CustomTakeDamage(DamageAmount, DamageCauser, AttackType);

	UE_LOG(LogTemp, Warning, TEXT("DarkKnight Damaged!"));

	if (!bDying && bShouldDrawWeapon) {
		GetWorldTimerManager().ClearTimer(SearchTimer);
		APlayerCharacter* Player = Cast<APlayerCharacter>(DamageCauser);
		Target = Player;

		// 강제로 ImpactState로 전환되는 것을 막는다. (데미지는 들어감)
		// Non-Battle Impact, DrawAnimation 모두 끝나면 false로 바뀐다.
		bAvoidImpactState = true;
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
