// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Potion.h"

APotion::APotion()
{

}

void APotion::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	const FString PotionTablePath{ "DataTable'/Game/_Game/DataTable/PotionDataTable.PotionDataTable'" };
	UDataTable* PotionTableObject = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, *PotionTablePath));

	if (PotionTableObject) {
		FPotionDataTable* PotionDataRow = nullptr;
		switch (PotionTier)
		{
		case 1:
			PotionDataRow = PotionTableObject->FindRow<FPotionDataTable>(FName("1"), TEXT(""));
			break;
		default:
			break;
		}

		if (PotionDataRow) {
			ItemName = PotionDataRow->PotionName;
			ItemIcon = PotionDataRow->PotionIcon;
			ItemMesh->SetSkeletalMesh(PotionDataRow->PotionMesh);

			RecoveryAmount = PotionDataRow->RecoveryAmount;
		}
	}
}
