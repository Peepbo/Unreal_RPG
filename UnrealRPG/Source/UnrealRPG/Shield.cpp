// Fill out your copyright notice in the Description page of Project Settings.


#include "Shield.h"

void AShield::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	const FString ShieldTablePath{ "DataTable'/Game/_Game/DataTable/ShieldDataTable.ShieldDataTable'" };
	UDataTable* ShieldTableObject = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, *ShieldTablePath));

	if (ShieldTableObject) {
		FShieldDataTable* ShieldDataRow = nullptr;
		switch (ShieldTier)
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
		}
	}
}