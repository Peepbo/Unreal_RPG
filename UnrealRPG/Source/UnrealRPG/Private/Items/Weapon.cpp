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

	if (WeaponTableObject)
	{
		FWeaponDataTable* WeaponDataRow = nullptr;
		switch (WeaponType)
		{
		case EWeaponType::EWT_OneHandWeapon:
			WeaponDataRow = WeaponTableObject->FindRow<FWeaponDataTable>(FName("OneHandSword"), TEXT(""));
			break;
		case EWeaponType::EWT_TwoHandWeapon:
			WeaponDataRow = WeaponTableObject->FindRow<FWeaponDataTable>(FName("TwoHandSword"), TEXT(""));
			break;
		}

		if (WeaponDataRow) 
		{
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

	// Hit??? ????????? ???
	if (bHit) 
	{
		// Actor??? nullptr??? ?????????
		if (HitResult.Actor != nullptr)
		{
			// ??? ??? AEnemy??? Cast??? ???????????????.
			auto Enemy = Cast<AEnemy>(HitResult.Actor);

			// Enemy??? Cast??? ??????????????? ?????? ???
			if (Enemy) 
			{
				// Enemy??? ???????????? ?????? ??? ?????? ????????????
				if (Enemy->DamageableState())
				{
					// ?????? ????????? ????????? ?????? ?????????????????? ??????, ???????????? ????????? ?????????.
					
					if (Character) 
					{
						Character->AddFunctionToDamageTypeResetDelegate(Enemy, FName("ResetDamageState"));
					}

					Enemy->CustomApplyDamage(
						Enemy,
						Damage,
						Character,
						EAttackType::EAT_Light
					);

					// ?????? ???????????? ????????? ??? ?????? ????????? ???????????? ????????????.
					if (Enemy->GetBloodParticle()) 
					{
						UGameplayStatics::SpawnEmitterAtLocation(
							GetWorld(),
							Enemy->GetBloodParticle(),
							HitResult.ImpactPoint,
							UKismetMathLibrary::MakeRotFromX(HitResult.ImpactNormal),
							true);
					}

					// ?????? ???????????? ????????? ???????????? ???????????????
					if (Enemy->GetDying())
					{
						if (Enemy->GetLockOn())
						{
							// KillEnemy??? ???????????? Lock-On??? ?????? ?????? ??????????????? ????????????.
							Character->ResetLockOn();
						}
					}
				}
			}
		}
	}
}