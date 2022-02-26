// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "Components/BoxComponent.h"

AWeapon::AWeapon()
{
	WeaponCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("Weapon Box"));
	WeaponCollision->SetupAttachment(GetRootComponent());
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
		}

		if (WeaponDataRow) {
			ItemName = WeaponDataRow->WeaponName;
			ItemIcon = WeaponDataRow->WeaponIcon;
			ItemMesh->SetSkeletalMesh(WeaponDataRow->WeaponMesh);
			WeaponType = WeaponDataRow->WeaponType;
			WeaponDamage = WeaponDataRow->WeaponDamage;
			WeaponChargedDamage = WeaponDataRow->WeaponChargedDamage;
		}
	}
}

void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
