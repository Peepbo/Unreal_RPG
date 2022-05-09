// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/LionKnight/LionKnight.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Magic/ProjectileMagic.h"
#include "Enemy/EnemyAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Player/PlayerCharacter.h"

ALionKnight::ALionKnight():
	ProjectileDamage(50.f),
	bPrepareBattle(false)
{
	GroundedWeapon = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GroundedWeapon"));
	bUseMagicCharacter = true;
	bAutoCombatReset = false;
}

void ALionKnight::BeginPlay()
{
	Super::BeginPlay();

	AD = 30.f;
	MaximumHP = 2000.f;
	HP = MaximumHP;

	ChangeEnemySize(EEnemySize::EES_Large);

	GetCharacterMovement()->MaxWalkSpeed = MaximumSprintSpeed;

	FTransform GroundedWeaponTransform{ GetMesh()->GetSocketTransform("GroundedWeaponSocket") };
	GroundedWeapon->SetWorldTransform(GroundedWeaponTransform);
}

void ALionKnight::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ALionKnight::ResetCombat()
{
	// 만약 보스전을 치뤘는데 패배했을 때 호출
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

void ALionKnight::UseMagic()
{
	if (ProjectileMagic)
	{
		const FVector MagicStartLoc{ GetActorLocation() + (GetActorForwardVector() * 150.f) };

		AProjectileMagic* SpawnMagic = GetWorld()->SpawnActor<AProjectileMagic>(ProjectileMagic, MagicStartLoc, GetActorRotation());
		SpawnMagic->InitMagic(this, ProjectileDamage);
	}
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
}
