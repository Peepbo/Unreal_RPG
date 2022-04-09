// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Item.h"
#include "Player/PlayerCharacter.h"

// Sets default values
AItem::AItem():
	ItemName("Default"),
	ItemDescription("None"),
	ItemType(EItemType::EIT_MAX)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	ItemMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ItemMesh"));
	SetRootComponent(ItemMesh);
}

// Called when the game starts or when spawned
void AItem::BeginPlay()
{
	Super::BeginPlay();
	
}

void AItem::SetCharacter(APlayerCharacter* Char)
{
	Character = Char;
	CharacterController = Char->GetController();
}

