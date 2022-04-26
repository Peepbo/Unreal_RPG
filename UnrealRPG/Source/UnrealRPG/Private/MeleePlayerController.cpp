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
