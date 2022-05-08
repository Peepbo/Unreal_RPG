// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Weapon.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Enemy/Enemy.h"
#include "Kismet/GameplayStatics.h"
#include "Player/PlayerCharacter.h"
#include "Sound/SoundCue.h"
#include "Kismet/KismetMathLibrary.h"
//

AWeapon::AWeapon() :
	WeaponTopSocketName(TEXT("TopSocket")),
	WeaponBottomSocketName(TEXT("BottomSocket")),
	AttackType(EWeaponAttackType::EWAT_Normal)
{
}

void AWeapon::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	const FString WeaponTablePath{ "DataTable'/Game/_Game/DataTable/WeaponDataTable.WeaponDataTable'" };
	UDataTable* WeaponTableObject = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, *WeaponTablePath));

	if (WeaponTableObject) {
		FWeaponDataTable* WeaponDataRow = nullptr;
		switch (WeaponType)
		{
		case EWeaponType::EWT_OneHandWeapon:
			WeaponDataRow = WeaponTableObject->FindRow<FWeaponDataTable>(FName("OneHandSword"), TEXT(""));
			break;
		case EWeaponType::EWT_TwoHandWeapon:
			WeaponDataRow = WeaponTableObject->FindRow<FWeaponDataTable>(FName("TwoHandSword"), TEXT(""));
			break;
			//case EWeaponType::EWT
		}

		if (WeaponDataRow) {
			ItemName = WeaponDataRow->WeaponName;
			ItemIcon = WeaponDataRow->WeaponIcon;
			ItemMesh->SetSkeletalMesh(WeaponDataRow->WeaponMesh);
			WeaponType = WeaponDataRow->WeaponType;
			NormalDamage = WeaponDataRow->NormalDamage;
			ChargedDamage = WeaponDataRow->ChargedDamage;
			DashDamage = WeaponDataRow->DashDamage;
			ExecutionFirstDamage = WeaponDataRow->ExecutionFirstDamage;
			ExecutionSecondDamage = WeaponDataRow->ExecutionSecondDamage;

			NormalAttackStamina = WeaponDataRow->NormalAttackStamina;
			DashAttackStamina = WeaponDataRow->DashAttackStamina;
			ChargedAttackStamina = WeaponDataRow->ChargedAttackStamina;
		}
	}
}

float AWeapon::GetDamage(EWeaponAttackType Type)
{
	switch (Type)
	{
	case EWeaponAttackType::EWAT_Normal:
		return NormalDamage;
	case EWeaponAttackType::EWAT_Charged:
		return ChargedDamage;
	case EWeaponAttackType::EWAT_Dash:
		return DashDamage;
	}

	UE_LOG(LogTemp, Warning, TEXT("AttackType Not Found"));
	return 0.0f;
}

void AWeapon::InitAttackData(EWeaponAttackType Type, bool bDebugVisible)
{
	AttackType = Type;

	AttackDelegate.Unbind();
	AttackDelegate.BindUFunction(
		this,
		FName("SwingWeapon"),
		bDebugVisible);
}

float AWeapon::GetRequiredStamina(EWeaponAttackType Type)
{
	switch (Type)
	{
	case EWeaponAttackType::EWAT_Normal:
		return NormalAttackStamina;
	case EWeaponAttackType::EWAT_Charged:
		return ChargedAttackStamina;
	case EWeaponAttackType::EWAT_Dash:
		return DashAttackStamina;
	}

	return 0.0f;
}

bool AWeapon::WeaponTraceSingle(bool bDebugVisible, FHitResult& OutHit)
{
	const FVector TopSocketLoc{ ItemMesh->GetSocketLocation(WeaponTopSocketName) };
	const FVector BottomSocketLoc{ ItemMesh->GetSocketLocation(WeaponBottomSocketName) };

	const bool bHit = UKismetSystemLibrary::SphereTraceSingleForObjects(
		this,
		BottomSocketLoc,
		TopSocketLoc,
		50.f,
		// Pawn
		{ EObjectTypeQuery::ObjectTypeQuery3 },
		false,
		{ Character },
		bDebugVisible? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None,
		OutHit,
		true
	);

	return bHit;
}

void AWeapon::SwingWeapon(bool bDebugVisible)
{
	FHitResult HitResult;
	const bool bHit{ WeaponTraceSingle(bDebugVisible,HitResult) };
	const float Damage{ GetDamage(AttackType) };

	// Hit이 되었을 때
	if (bHit) {
		// Actor가 nullptr이 아니면
		if (HitResult.Actor != nullptr) {
			// 한 번 AEnemy로 Cast를 시도해본다.
			auto Enemy = Cast<AEnemy>(HitResult.Actor);

			// Enemy로 Cast가 성공적으로 됬을 때
			if (Enemy) {
				// Enemy가 데미지를 입을 수 있는 상태라면
				if (Enemy->DamageableState()) {
					// 상태 초기화 함수를 리셋 델리게이트에 넣고, 몬스터에 피해를 가한다.
					
					if (Character) {
						Character->AddFunctionToDamageTypeResetDelegate(Enemy, FName("ResetDamageState"));
					}

					Enemy->CustomApplyDamage(
						Enemy,
						Damage,
						Character,
						EAttackType::EAT_Light
					);

					// 피해 파티클이 존재할 때 타격 위치에 파티클을 생성한다.
					if (Enemy->GetBloodParticle()) {
						UGameplayStatics::SpawnEmitterAtLocation(
							GetWorld(),
							Enemy->GetBloodParticle(),
							HitResult.ImpactPoint,
							UKismetMathLibrary::MakeRotFromX(HitResult.ImpactNormal),
							true);
					}

					// 방금 일격으로 락온된 몬스터가 사망했다면
					if (Enemy->GetDying())
					{
						if (Enemy->GetLockOn())
						{
							// KillEnemy를 호출하여 Lock-On과 같은 특수 상호작용을 리셋한다.
							Character->ResetLockOn();
						}
					}
				}
			}
		}
	}
}