// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "Components/BoxComponent.h"

AWeapon::AWeapon()
{
	WeaponCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("Weapon Box"));
	WeaponCollision->SetupAttachment(GetRootComponent());
}

void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
