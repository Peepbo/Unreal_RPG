// Fill out your copyright notice in the Description page of Project Settings.


#include "MeleePlayerController.h"
#include "Blueprint/UserWidget.h"
#include "Player/PlayerCharacter.h"

AMeleePlayerController::AMeleePlayerController()
{

}

void AMeleePlayerController::BeginPlay()
{
	Super::BeginPlay();
}

void AMeleePlayerController::CreateHubOverlay()
{
	// 이미 오버레이를 만들었다면 스킵한다.
	if (HUDOverlay)return;

	if (HubOverlayClass)
	{
		HUDOverlay = CreateWidget<UUserWidget>(this, HubOverlayClass);
		if (HUDOverlay)
		{
			HUDOverlay->AddToViewport();
			HUDOverlay->SetVisibility(ESlateVisibility::Visible);
		}
	}
}
