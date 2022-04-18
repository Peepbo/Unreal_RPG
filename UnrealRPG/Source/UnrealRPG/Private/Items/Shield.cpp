// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Shield.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Enemy/Enemy.h"
#include "Kismet/GameplayStatics.h"
#include "Player/PlayerCharacter.h"
#include "Sound/SoundCue.h"

AShield::AShield() 
{
}

void AShield::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	const FString ShieldTablePath{ "DataTable'/Game/_Game/DataTable/ShieldDataTable.ShieldDataTable'" };
	UDataTable* ShieldTableObject = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, *ShieldTablePath));

	if (ShieldTableObject) {
		FShieldDataTable* ShieldDataRow = nullptr;
		switch (ShieldItemNumber)
		{
		case 1:
			ShieldDataRow = ShieldTableObject->FindRow<FShieldDataTable>(FName("1"),TEXT(""));
			break;
		case 2:
			ShieldDataRow = ShieldTableObject->FindRow<FShieldDataTable>(FName("2"),TEXT(""));
			break;
		}

		if (ShieldDataRow) {
			ItemName = ShieldDataRow->ShieldName;
			ItemIcon = ShieldDataRow->ShieldIcon;
			ItemMesh->SetSkeletalMesh(ShieldDataRow->ShieldMesh);
			const float ShieldScale = ShieldDataRow->ShieldScale;
			const FVector ShiledSize{ ShieldScale ,ShieldScale ,ShieldScale };
			ItemMesh->SetRelativeScale3D(ShiledSize);

			ShieldDefence = ShieldDataRow->ShieldDefence;
			DefenceDegree = ShieldDataRow->DefenceDegree;
			AttackDamage = ShieldDataRow->AttackDamage;
		}
	}
}

void AShield::InitPushShiledData(bool bDebugVisible)
{
	ShieldDelegate.Unbind();
	ShieldDelegate.BindUFunction(
		this,
		FName("PushShield"),
		bDebugVisible);
}

bool AShield::ShieldTraceSingle(bool bDebugVisible, FHitResult& OutHit)
{
	const FVector ShieldSocketLoc{ ItemMesh->GetSocketLocation(ShieldSocketName) };

	const bool bHit = UKismetSystemLibrary::SphereTraceSingleForObjects(
		this,
		ShieldSocketLoc,
		ShieldSocketLoc,
		50.f,
		// Pawn
		{ EObjectTypeQuery::ObjectTypeQuery3 },
		false,
		{ Character },
		bDebugVisible ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None,
		OutHit,
		true
	);
	
	return bHit;
}

void AShield::PushShield(bool bDebugVisible)
{
	FHitResult HitResult;
	const bool bHit{ ShieldTraceSingle(bDebugVisible, HitResult) };
	
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
						AttackDamage,
						Character,
						EAttackType::EAT_Light
					);

					// 피해 파티클이 존재할 때 타격 위치에 파티클을 생성한다.
					if (Enemy->GetBloodParticle()) {
						UGameplayStatics::SpawnEmitterAtLocation(
							GetWorld(),
							Enemy->GetBloodParticle(),
							HitResult.ImpactPoint);
					}
					// 피해 사운드가 존재할 때 타격 위치에 사운드를 생성한다.
					if (Enemy->GetBloodSound())
					{
						UGameplayStatics::PlaySoundAtLocation(
							this,
							Enemy->GetBloodSound(),
							HitResult.ImpactPoint);
					}

					// 방금 일격으로 락온된 몬스터가 사망했다면
					if (Enemy->GetDying())
					{
						if (Enemy->GetLastBloodSound())
						{
							UGameplayStatics::PlaySoundAtLocation(
								this,
								Enemy->GetLastBloodSound(),
								HitResult.ImpactPoint);
						}

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
